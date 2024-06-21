#include "spaceshield.h"

#include <iostream>

// ImageMagick dependency for jpg
#include "CImg.h"

#include "hidapi.h"

/*
	Буфер для работы по modbus
*/
unsigned char buf[64];

/*
	Поверхность экрана (используется для оптимизации отрисовки)
*/
display_color surface[128][64];

enum function_code
{
	GET_BUTTONS_STATE_FUNCTION_CODE = 0x01,
	SET_LED_COLOR_FUNCTION_CODE = 0x02,
	GET_RESISTOR_VALUE_FUNCTION_CODE = 0x03,
	DRAW_PIXEL_FUNCTION_CODE = 0x04,
	DISPLAY_FILL_FUNCTION_CODE = 0x05,
	DRAW_LINE_FUNCTION_CODE = 0x06,
	DRAW_POLY_PIXEL_FUNCTION_CODE = 0x07
};

void set_function_code(unsigned char code)
{
	buf[0] = code;
}

hid_device *handle;

void init_shield()
{
	if (hid_init())
	{
		printf("hid init error\n");

		return;
	}

	// Set up the command buffer.
	memset(buf, 0x00, sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	handle = hid_open(0x1234, 0x0001, NULL);

	if (!handle)
	{
		printf("unable to open device\n");
		return;
	}

	// Устанавливаем стандартные значения
	set_led_color(LED_GREEN);

	display_fill(DISPLAY_COLOR_BLACK);
}

void close_shield()
{
	// Устанавливаем стандартные значения
	set_led_color(LED_RED);
	display_fill(DISPLAY_COLOR_BLACK);

	hid_exit();
}

void set_led_color(led_color color)
{
	set_function_code(SET_LED_COLOR_FUNCTION_CODE);

	buf[1] = color.r; //
	buf[2] = 0;		  // 2 byte = uint16_t = power of light color 1 // при изменении ломается регулировка ярокости светодиода
	buf[3] = color.b; //
	buf[4] = 0;		  // 2 byte = uint16_t = power of light color 2 // при изменении ломается регулировка ярокости светодиода
	buf[5] = color.g; //
	buf[6] = 0;		  // 2 byte = uint16_t = power of light color 3 // при изменении ломается регулировка ярокости светодиода

	if (hid_send_feature_report(handle, buf, 7) == -1) // отправка информации на устройство (7 байт)
		printf("hid_write error.\n");
}

unsigned char get_resistor_value()
{
	set_function_code(GET_RESISTOR_VALUE_FUNCTION_CODE);

	if (hid_get_feature_report(handle, buf, sizeof(buf)) < 0)
	{
		printf("Unable to get a feature report.\n%ls", hid_error(handle));

		return 0;
	}

	return buf[2] * 16; // buf[0] - номер функции, buf[1] - слишком точное значение
}

void display_fill(display_color color)
{
	set_function_code(DISPLAY_FILL_FUNCTION_CODE);
	buf[1] = color;

	for (size_t i = 0; i < 64; i++)
		for (size_t j = 0; j < 128; j++)
			surface[j][i] = color;

	if (hid_send_feature_report(handle, buf, 2) == -1)
		printf("hid_write error.\n");
}

void draw_pixel(display_point point, display_color color)
{
	if (surface[point.x][point.y] == color)
		return;
	surface[point.x][point.y] = color;

	set_function_code(DRAW_PIXEL_FUNCTION_CODE);
	buf[1] = point.x;
	buf[2] = point.y;
	buf[3] = color;

	if (hid_send_feature_report(handle, buf, 4) == -1)
		printf("hid_write error.\n");
}

void draw_poly_pixel(int count, unsigned char *pixels, bool allow_drop)
{
	clock_t frame_time = clock();
	bool odd = true;
	int cur_packet_size = 0,
		lines = count / 128, // + count % 128
		i = 0, j = 0;

	set_function_code(DRAW_POLY_PIXEL_FUNCTION_CODE);

	for (size_t i = 0; i < lines; i += 2)
		for (size_t j = 0; j < 128 * 3; j += 3)
		{
			if (clock() - frame_time > 15)
			{
				std::cout << "Frame drop!" << std::endl;
				return;
			}

			if (surface[pixels[j + i * 128 * 3]][pixels[j + 1 + i * 128 * 3]] != pixels[j + 2 + i * 128 * 3]) // нужно ли изменять пиксель?
			{
				surface[pixels[j + i * 128 * 3]][pixels[j + 1 + i * 128 * 3]] = (display_color)pixels[j + 2 + i * 128 * 3];
				buf[2 + cur_packet_size * 3] = pixels[j + i * 128 * 3];
				buf[3 + cur_packet_size * 3] = pixels[j + 1 + i * 128 * 3];
				buf[4 + cur_packet_size * 3] = pixels[j + 2 + i * 128 * 3];
				cur_packet_size++;
			}

			if (cur_packet_size == 20)
			{
				buf[1] = cur_packet_size;
				if (hid_send_feature_report(handle, buf, 62) == -1)
					printf("hid_write error.\n");

				memset(buf, 0x00, sizeof(buf));
				set_function_code(DRAW_POLY_PIXEL_FUNCTION_CODE);

				cur_packet_size = 0;
			}
		}

	for (int i = lines - 1; i > 0; i -= 2)
		for (size_t j = 0; j < 128 * 3; j += 3)
		{
			if (clock() - frame_time > 15)
			{
				std::cout << "Frame drop!" << std::endl;
				return;
			}

			if (surface[pixels[j + i * 128 * 3]][pixels[j + 1 + i * 128 * 3]] != pixels[j + 2 + i * 128 * 3]) // нужно ли изменять пиксель?
			{
				surface[pixels[j + i * 128 * 3]][pixels[j + 1 + i * 128 * 3]] = (display_color)pixels[j + 2 + i * 128 * 3];
				buf[2 + cur_packet_size * 3] = pixels[j + i * 128 * 3];
				buf[3 + cur_packet_size * 3] = pixels[j + 1 + i * 128 * 3];
				buf[4 + cur_packet_size * 3] = pixels[j + 2 + i * 128 * 3];
				cur_packet_size++;
			}

			if (cur_packet_size == 20)
			{
				buf[1] = cur_packet_size;
				if (hid_send_feature_report(handle, buf, 62) == -1)
					printf("hid_write error.\n");

				memset(buf, 0x00, sizeof(buf));
				set_function_code(DRAW_POLY_PIXEL_FUNCTION_CODE);

				cur_packet_size = 0;
			}
		}

	if (cur_packet_size)
	{
		buf[1] = cur_packet_size;
		if (hid_send_feature_report(handle, buf, 62) == -1)
			printf("hid_write error.\n");
	}
}

void draw_line(display_point start, display_point end, display_color color)
{
	set_function_code(DRAW_LINE_FUNCTION_CODE);
	buf[1] = start.x;
	buf[2] = start.y;
	buf[3] = end.x;
	buf[4] = end.y;
	buf[5] = color;

	if (hid_send_feature_report(handle, buf, 6) == -1)
		printf("hid_write error.\n");
}

void draw_image(const char *path)
{
	cimg_library::CImg<unsigned char> img(path);
	img.resize(128, 64, 1, 1, 5);

	unsigned char pixels[128 * 64 * 3];

	int medium = 0;
	for (size_t j = 0; j < 64; j++)
	{
		for (size_t i = 0; i < 128; i++)
			medium += (int)img(i, j);
	}
	medium /= 8192;

	display_point point;
	display_color color;
	for (size_t j = 0; j < 64; j++)
	{
		point.y = j;

		for (size_t i = 0; i < 128; i++)
		{
			(img(i, j) > medium) ? (color = DISPLAY_COLOR_WHITE) : (color = DISPLAY_COLOR_BLACK);
			point.x = i;

			pixels[0 + j * 128 * 3 + i * 3] = point.x;
			pixels[1 + j * 128 * 3 + i * 3] = point.y;
			pixels[2 + j * 128 * 3 + i * 3] = color;
		}
	}

	draw_poly_pixel(128 * 64, pixels, true);
}

unsigned char get_buttons_state()
{
	set_function_code(GET_BUTTONS_STATE_FUNCTION_CODE);

	if (hid_get_feature_report(handle, buf, sizeof(buf)) < 0)
	{
		printf("Unable to get a feature report.\n%ls", hid_error(handle));

		return 0;
	}

	return buf[1];
}
