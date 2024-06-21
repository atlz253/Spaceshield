#include "spaceshield.h"

#include <iostream>

#include "hidapi.h"

/*
	Буфер для работы по modbus
*/
unsigned char buf[256];

/*
	Поверхность экрана (используется для оптимизации отрисовки)
*/
unsigned char surface[128][64];

enum function_code
{
	GET_BUTTONS_STATE_FUNCTION_CODE = 0x01,
	SET_LED_COLOR_FUNCTION_CODE = 0x02,
	GET_RESISTOR_VALUE_FUNCTION_CODE = 0x03,
	DRAW_PIXEL_FUNCTION_CODE = 0x04
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
	clear_display();
}

void close_shield()
{
	// Устанавливаем стандартные значения
	set_led_color(LED_RED);
	clear_display();

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

void clear_display()
{
	display_point point;
	point.color = 0;

	for (size_t j = 0; j < 64; j++)
	{
		for (size_t i = 0; i < 128; i++)
		{
			surface[i][j] = 1; // ставим единицу, чтобы гарантированно пиксель потух

			point.x = i;
			point.y = j;

			draw_pixel(point);
		}
	}
}

void draw_pixel(display_point point)
{
	if (surface[point.x][point.y] == point.color)
		return;

	surface[point.x][point.y] = point.color;
	set_function_code(DRAW_PIXEL_FUNCTION_CODE);

	buf[1] = point.x;
	buf[2] = point.y;
	buf[3] = point.color;

	if (hid_send_feature_report(handle, buf, 4) == -1)
		printf("hid_write error.\n");
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
