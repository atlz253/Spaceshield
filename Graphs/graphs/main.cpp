#include <ctime>
#include <iostream>
#include <windows.h>
#include <cmath>

#include "spaceshield.h"

#define CONSOLE_CLEAR system("cls") // очищаем консоль
#define CONSOLE_PAUSE system("pause") // пауза в консоли

/* Состояния программы */
enum class state
{
	menu,
	graph_line,
	graph_parabola,
	graph_hyperbola,
	graph_cubic,
};
state cur_state = state::menu; // текущее состояние программы


int cur_x = -64, // от какого x строится график
cur_y = 0;   // от какого y строится график

bool work = true; // работает ли программа?

/*
	На экране кординаты точки (0, 0) - левых верхний угол.
	В графиках принято откладывать данную точку в левом нижнем угле.
*/
void draw_pixel_decart(display_point point)
{
	point.y = 62 - point.y;

	draw_pixel(point);
}

/* Алгоритм Брезенхэма для рисования линий между двумя точками */
void draw_Line(int x1, int y1, int x2, int y2, unsigned char color) {
	int deltaX = abs(x2 - x1),
		deltaY = abs(y2 - y1),
		signX = ((x1 < x2) ? 1 : -1),
		signY = ((y1 < y2) ? 1 : -1),
		error = deltaX - deltaY,
		error2;

	draw_pixel_decart({ (unsigned char)x2, (unsigned char)y2, color });
	while ((x1 != x2) || (y1 != y2))
	{
		draw_pixel_decart({ (unsigned char)x1, (unsigned char)y1, color });
		error2 = error * 2;
		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}

		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}
}

/* Рисуем гиперболу */
void hyperbola_graph()
{
	float k = 0;

	CONSOLE_CLEAR;

#pragma region Inputs
	while (k == 0)
	{
		std::cout << "Введите коэффициент k: ";
		std::cin >> k;

		if (k == 0)
		{
			std::cout << "Введите коэффициент не равный нулю";
			CONSOLE_PAUSE;
		}
	}
#pragma endregion

	unsigned char last_resistor_value = 0; // последнее значение резистора
	clock_t last_resistor_update = 0, // время последнего обновления резистора
		last_buttons_check = 0;  // время последней проверки кнопок


	while (true)
	{
		if (clock() - last_buttons_check > 1000)
		{
			last_buttons_check = clock();

			if (get_buttons_state() == FIRST_BUTTON_DOWN)
			{
				cur_state = state::menu;
				return;
			}
		}

		if (clock() - last_resistor_update < 1000)
			continue;
		last_resistor_update = clock();

		if (abs(get_resistor_value() - last_resistor_value) < 5)
			continue;
		last_resistor_value = get_resistor_value();

		clear_display();
		int ratio = get_resistor_value() / 10 + 1;

		float y_cords[128] = { 0 };

		for (int x = 0; x < 128; x++)
		{
			int map_x = x + cur_x;
			y_cords[x] = (k * ratio) / map_x;
		}

		int start = 0;
		while (y_cords[start] < 0)
			start++;

		if (start > 0)
		{
			start--;
			y_cords[start] = 0;
		}

		for (unsigned char x = start; x < 127; x++)
		{
			if (y_cords[x] > 128 || y_cords[x] < 0 || y_cords[x + 1] > 128 || y_cords[x + 1] < 0)
				break;

			draw_Line(x, y_cords[x], x + 1, y_cords[x + 1], 1);
		}

	}

	CONSOLE_PAUSE;
}

/* Рисуем кубическую параболу */
void cubic_graph()
{
	float a = 0, b, c, d;

	CONSOLE_CLEAR;

#pragma region Inputs
	while (a == 0)
	{
		std::cout << "Введите коэффициент a: ";
		std::cin >> a;

		if (a == 0)
		{
			std::cout << "Введите коэффициент не равный нулю";
			CONSOLE_PAUSE;
		}
	}

	std::cout << "Введите коэффициент b: ";
	std::cin >> b;

	std::cout << "Введите коэффициент c: ";
	std::cin >> c;

	std::cout << "Введите коэффициент d: ";
	std::cin >> d;
#pragma endregion

	unsigned char last_resistor_value = 0; // последнее значение резистора
	clock_t last_resistor_update = 0, // время последнего обновления резистора
		last_buttons_check = 0;  // время последней проверки кнопок


	while (true)
	{
		if (clock() - last_buttons_check > 1000)
		{
			last_buttons_check = clock();

			if (get_buttons_state() == FIRST_BUTTON_DOWN)
			{
				cur_state = state::menu;
				return;
			}
		}

		if (clock() - last_resistor_update < 1000)
			continue;
		last_resistor_update = clock();

		if (abs(get_resistor_value() - last_resistor_value) < 5)
			continue;
		last_resistor_value = get_resistor_value();

		clear_display();
		int ratio = get_resistor_value() / 10 + 1;

		float y_cords[128] = { 0 };

		for (int x = 0; x < 128; x++)
		{
			int map_x = x + cur_x;
			y_cords[x] = (a / ratio) * map_x * map_x * map_x + b * map_x * map_x + c * map_x + d;
		}

		int start = 0;
		while (y_cords[start] < 0)
			start++;

		if (start > 0)
		{
			start--;
			y_cords[start] = 0;
		}

		for (unsigned char x = start; x < 127; x++)
		{
			if (y_cords[x] > 128 || y_cords[x] < 0)
				break;

			draw_Line(x, y_cords[x], x + 1, y_cords[x + 1], 1);
		}

	}

	CONSOLE_PAUSE;
}


/* Рисуем параболу */
void parabola_graph()
{
	float a = 0, b, c;

	CONSOLE_CLEAR;

#pragma region Inputs
	while (a == 0)
	{
		std::cout << "Введите коэффициент a: ";
		std::cin >> a;

		if (a == 0)
		{
			std::cout << "Введите коэффициент не равный нулю";
			CONSOLE_PAUSE;
		}
	}

	std::cout << "Введите коэффициент b: ";
	std::cin >> b;

	std::cout << "Введите коэффициент c: ";
	std::cin >> c;
#pragma endregion

	unsigned char last_resistor_value = 0; // последнее значение резистора
	clock_t last_resistor_update = 0, // время последнего обновления резистора
		last_buttons_check = 0;  // время последней проверки кнопок


	while (true)
	{
		if (clock() - last_buttons_check > 1000)
		{
			last_buttons_check = clock();

			if (get_buttons_state() == FIRST_BUTTON_DOWN)
			{
				cur_state = state::menu;
				return;
			}
		}

		if (clock() - last_resistor_update < 1000)
			continue;
		last_resistor_update = clock();

		if (abs(get_resistor_value() - last_resistor_value) < 5)
			continue;
		last_resistor_value = get_resistor_value();

		clear_display();
		int ratio = get_resistor_value() / 10 + 1;

		float y_cords[128] = { 0 };

		for (int x = 0; x < 128; x++)
		{
			int map_x = x + cur_x;
			y_cords[x] = (a / ratio) * map_x * map_x + b * map_x + c;
		}

		int start = 0;
		while (y_cords[start] > 128)
			start++;

		if (start > 0)
		{
			start--;
			y_cords[start] = 127;
		}

		for (unsigned char x = start; x < 127; x++)
		{
			if (y_cords[x] > 128 || y_cords[x] < 0)
				break;

			draw_Line(x, y_cords[x], x + 1, y_cords[x + 1], 1);
		}

	}

	CONSOLE_PAUSE;
}

/* Рисуем прямую */
void line_graph()
{
	float a, b;

	CONSOLE_CLEAR;

#pragma region Inputs
	std::cout << "Введите коэффициент a: ";
	std::cin >> a;

	std::cout << "Введите коэффициент b: ";
	std::cin >> b;
#pragma endregion

	unsigned char last_resistor_value = 0; // последнее значение резистора
	clock_t last_resistor_update = 0, // время последнего обновления резистора
		last_buttons_check = 0;  // время последней проверки кнопок


	while (true)
	{
		if (clock() - last_buttons_check > 1000)
		{
			last_buttons_check = clock();

			if (get_buttons_state() == FIRST_BUTTON_DOWN)
			{
				cur_state = state::menu;
				return;
			}
		}

		if (clock() - last_resistor_update < 1000)
			continue;
		last_resistor_update = clock();

		if (abs(get_resistor_value() - last_resistor_value) < 5)
			continue;
		last_resistor_value = get_resistor_value();

		clear_display();
		int ratio = get_resistor_value() / 10 + 1;

		float start_x = cur_x,
			start_y = (a / ratio) * start_x + b,
			end_x = 127 + cur_x,
			end_y = (a / ratio) * end_x + b;

		if (start_y > 128)
			start_y = 127;
		else if (start_y < 0)
			start_y = 0;

		if (end_y > 128)
			end_y = 127;
		else if (end_y < 0)
			end_y = 0;

		draw_Line(0, start_y, 127, end_y, 1);
	}

	CONSOLE_PAUSE;
}

void graph_menu()
{
	int choice;

	CONSOLE_CLEAR;
	clear_display();

	std::cout << "1. y = ax + b" << std::endl
		<< "2. y = ax^2 + bx + c" << std::endl
		<< "3. y = ax^3 + bx + c" << std::endl
		<< "4. y = k/x" << std::endl
		<< "0. выход" << std::endl;
	std::cin >> choice;

	switch (choice)
	{
	case 0:
		work = false;
		break;
	case 1:
		cur_state = state::graph_line;
		break;
	case 2:
		cur_state = state::graph_parabola;
		break;
	case 3:
		cur_state = state::graph_cubic;
		break;
	case 4:
		cur_state = state::graph_hyperbola;
		break;
	}
}

void start()
{
	while (work)
		switch (cur_state)
		{
		case state::menu:
			graph_menu();
			break;
		case state::graph_parabola:
			parabola_graph();
			break;
		case state::graph_line:
			line_graph();
			break;
		case state::graph_cubic:
			cubic_graph();
			break;
		case state::graph_hyperbola:
			hyperbola_graph();
			break;
		}
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	init_shield();
	srand(time(NULL));

	start();

	close_shield();
	return 0;
}
