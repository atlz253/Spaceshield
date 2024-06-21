#include <ctime>
#include <iostream>
#include <windows.h>

#include "spaceshield.h"

/* Возможные состояния игрового поля */
enum class state
{
	EMPTY,
	CROSS,
	CIRCLE
};
state map[3][3] = { state::EMPTY }; // игровое состояние на данный момент

unsigned char last_map_index = 0, // последняя выбранная строка игрового поля на данный момент
last_resistor_value = 0; // последнее значение переменного резистра

bool game_state = true, // работает ли игра?
	 player = false, // true - крестики, false - нолики
	 arrowView = false; // нужно ли показывать указатель строки?

clock_t last_resistor_check = clock(), // последнее время проверки резистора
	last_buttons_check = clock(); // последнее время проверки кнопок

/* Рисование стрелки выбранной строки                 */
/* index - номер строки, на которую указывает стрелка */
void draw_arrow(unsigned char index)
{
	for (unsigned char i = 0; i < 7; i++)
	{
		/* Удаление с экрана старой стрелки */
		draw_pixel({ (unsigned char)(32 - i), (unsigned char)(10 + last_map_index * 20), 0 });
		draw_pixel({ (unsigned char)(32 - i), (unsigned char)(11 + last_map_index * 20), 0 });

		/* Рисование новой стрелки */
		draw_pixel({ (unsigned char)(32 - i), (unsigned char)(10 + index * 20), 1 });
		draw_pixel({ (unsigned char)(32 - i), (unsigned char)(11 + index * 20), 1 });
	}

	for (unsigned char i = 0; i < 3; i++)
	{
		/* Удаление с экрана старой стрелки */
		draw_pixel({ (unsigned char)(31 - i), (unsigned char)(9 + last_map_index * 20), 0 });
		draw_pixel({ (unsigned char)(31 - i), (unsigned char)(12 + last_map_index * 20), 0 });

		/* Рисование новой стрелки */
		draw_pixel({ (unsigned char)(31 - i), (unsigned char)(9 + index * 20), 1 });
		draw_pixel({ (unsigned char)(31 - i), (unsigned char)(12 + index * 20), 1 });
	}

	for (unsigned char i = 0; i < 2; i++)
	{
		/* Удаление с экрана старой стрелки */
		draw_pixel({ (unsigned char)(30 - i), (unsigned char)(8 + last_map_index * 20), 0 });
		draw_pixel({ (unsigned char)(30 - i), (unsigned char)(13 + last_map_index * 20), 0 });

		/* Рисование новой стрелки */
		draw_pixel({ (unsigned char)(30 - i), (unsigned char)(8 + index * 20), 1 });
		draw_pixel({ (unsigned char)(30 - i), (unsigned char)(13 + index * 20), 1 });
	}

	last_map_index = index;
}

/* Рисование крестика */
void draw_cross(display_point point)
{
	for (size_t i = 0; i < 18; i++)
	{
		draw_pixel({ (unsigned char)(36 + 20 * point.x + i), (unsigned char)(4 + 20 * point.y + i), 1 });
		draw_pixel({ (unsigned char)(52 + 20 * point.x - i), (unsigned char)(4 + 20 * point.y + i), 1 });
	}
}

/* Рисование нолика */
void draw_circle(display_point point)
{
	for (size_t i = 0; i < 6; i++)
	{
		draw_pixel({ (unsigned char)(42 + 20 * point.x + i), (unsigned char)(4 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(42 + 20 * point.x + i), (unsigned char)(20 + 20 * point.y), 1 });
	}

	for (size_t i = 0; i < 3; i++)
	{
		draw_pixel({ (unsigned char)(40 + 20 * point.x + i), (unsigned char)(5 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(40 + 20 * point.x + i), (unsigned char)(19 + 20 * point.y), 1 });

		draw_pixel({ (unsigned char)(47 + 20 * point.x + i), (unsigned char)(5 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(47 + 20 * point.x + i), (unsigned char)(19 + 20 * point.y), 1 });
	}

	for (size_t i = 0; i < 2; i++)
	{
		draw_pixel({ (unsigned char)(39 + 20 * point.x + i), (unsigned char)(6 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(39 + 20 * point.x + i), (unsigned char)(18 + 20 * point.y), 1 });
		
		draw_pixel({ (unsigned char)(49 + 20 * point.x + i), (unsigned char)(6 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(49 + 20 * point.x + i), (unsigned char)(18 + 20 * point.y), 1 });


		draw_pixel({ (unsigned char)(38 + 20 * point.x + i), (unsigned char)(7 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(38 + 20 * point.x + i), (unsigned char)(17 + 20 * point.y), 1 });

		draw_pixel({ (unsigned char)(50 + 20 * point.x + i), (unsigned char)(7 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(50 + 20 * point.x + i), (unsigned char)(17 + 20 * point.y), 1 });


		draw_pixel({ (unsigned char)(37 + 20 * point.x + i), (unsigned char)(8 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(37 + 20 * point.x + i), (unsigned char)(16 + 20 * point.y), 1 });

		draw_pixel({ (unsigned char)(51 + 20 * point.x + i), (unsigned char)(8 + 20 * point.y), 1 });
		draw_pixel({ (unsigned char)(51 + 20 * point.x + i), (unsigned char)(16 + 20 * point.y), 1 });
	}

	draw_pixel({ (unsigned char)(37 + 20 * point.x), (unsigned char)(9 + 20 * point.y), 1 });
	draw_pixel({ (unsigned char)(37 + 20 * point.x), (unsigned char)(15 + 20 * point.y), 1 });

	draw_pixel({ (unsigned char)(52 + 20 * point.x), (unsigned char)(9 + 20 * point.y), 1 });
	draw_pixel({ (unsigned char)(52 + 20 * point.x), (unsigned char)(15 + 20 * point.y), 1 });


	draw_pixel({ (unsigned char)(37 + 20 * point.x), (unsigned char)(10 + 20 * point.y), 1 });
	draw_pixel({ (unsigned char)(37 + 20 * point.x), (unsigned char)(14 + 20 * point.y), 1 });

	draw_pixel({ (unsigned char)(52 + 20 * point.x), (unsigned char)(10 + 20 * point.y), 1 });
	draw_pixel({ (unsigned char)(52 + 20 * point.x), (unsigned char)(14 + 20 * point.y), 1 });

	for (size_t i = 0; i < 5; i++)
	{
		draw_pixel({ (unsigned char)(36 + 20 * point.x), (unsigned char)(10 + 20 * point.y + i), 1 });
		draw_pixel({ (unsigned char)(53 + 20 * point.x), (unsigned char)(10 + 20 * point.y + i), 1 });
	}
}

void init_game()
{
	/* Очистка игрового поля */
	clear_display();
	for (size_t x = 0; x < 3; x++)
		for (size_t y = 0; y < 3; y++)
			map[x][y] = state::EMPTY;

	player = false;
	arrowView = true;

	/* Вертикальные линии */
	for (unsigned char i = 0; i < 60; i++)
	{
		draw_pixel({ 34, (unsigned char)(2 + i), 1 });
		draw_pixel({ 54, (unsigned char)(2 + i), 1 });
		draw_pixel({ 74, (unsigned char)(2 + i), 1 });
		draw_pixel({ 94, (unsigned char)(2 + i), 1 });
	}

	/* Горизонтальные линии */
	for (size_t i = 0; i < 60; i++)
	{
		draw_pixel({ (unsigned char)(34 + i), 2, 1 });
		draw_pixel({ (unsigned char)(34 + i), 22, 1 });
		draw_pixel({ (unsigned char)(34 + i), 42, 1 });
		draw_pixel({ (unsigned char)(34 + i), 62, 1 });
	}
}

/* Проверка условий победы */
void check_win()
{
	/* Победа крестика */
	if ((map[0][0] == state::CIRCLE && map[1][0] == state::CIRCLE && map[2][0] == state::CIRCLE) || 
		(map[0][1] == state::CIRCLE && map[1][1] == state::CIRCLE && map[2][1] == state::CIRCLE) ||
		(map[0][2] == state::CIRCLE && map[1][2] == state::CIRCLE && map[2][2] == state::CIRCLE) ||
		(map[0][0] == state::CIRCLE && map[0][1] == state::CIRCLE && map[0][2] == state::CIRCLE) ||
		(map[1][0] == state::CIRCLE && map[1][1] == state::CIRCLE && map[1][2] == state::CIRCLE) ||
		(map[2][0] == state::CIRCLE && map[2][1] == state::CIRCLE && map[2][2] == state::CIRCLE) ||
		(map[0][0] == state::CIRCLE && map[1][1] == state::CIRCLE && map[2][2] == state::CIRCLE) ||
		(map[2][0] == state::CIRCLE && map[1][1] == state::CIRCLE && map[0][2] == state::CIRCLE))
	{
		clear_display();
		draw_circle({1, 1});
		arrowView = false;
	}

	/* Победа нолика */
	if ((map[0][0] == state::CROSS && map[1][0] == state::CROSS && map[2][0] == state::CROSS) ||
		(map[0][1] == state::CROSS && map[1][1] == state::CROSS && map[2][1] == state::CROSS) ||
		(map[0][2] == state::CROSS && map[1][2] == state::CROSS && map[2][2] == state::CROSS) ||
		(map[0][0] == state::CROSS && map[0][1] == state::CROSS && map[0][2] == state::CROSS) ||
		(map[1][0] == state::CROSS && map[1][1] == state::CROSS && map[1][2] == state::CROSS) ||
		(map[2][0] == state::CROSS && map[2][1] == state::CROSS && map[2][2] == state::CROSS) ||
		(map[0][0] == state::CROSS && map[1][1] == state::CROSS && map[2][2] == state::CROSS) ||
		(map[2][0] == state::CROSS && map[1][1] == state::CROSS && map[0][2] == state::CROSS))
	{
		clear_display();
		draw_cross({ 1, 1 });
		arrowView = false;
	}

	/* Ничья */
	for (size_t x = 0; x < 3; x++)
		for (size_t y = 0; y < 3; y++)
			if (map[x][y] == state::EMPTY)
			{
				return;
			}
			else if (x == 2 && y == 2)
			{
				clear_display();
				draw_cross({ 0, 1 });
				draw_circle({ 2, 1 });
				arrowView = false;
			}
}

/* Проверка кнопок */
void check_buttons()
{
	if (clock() - last_buttons_check < 150)
		return;

	switch (get_buttons_state())
	{
	case FIRST_BUTTON_DOWN:
		if (map[0][last_map_index] == state::EMPTY)
		{
			player ? map[0][last_map_index] = state::CIRCLE : map[0][last_map_index] = state::CROSS;
			player ? draw_circle({ 0, last_map_index }) : draw_cross({0, last_map_index});
			player = !player;
			check_win();
		}
		break;
	case SECOND_BUTTON_DOWN:
		if (map[1][last_map_index] == state::EMPTY)
		{
			player ? map[1][last_map_index] = state::CIRCLE : map[1][last_map_index] = state::CROSS;
			player ? draw_circle({ 1, last_map_index }) : draw_cross({ 1, last_map_index });
			player = !player;
			check_win();
		}
		break;
	case THIRD_BUTTON_DOWN:
		if (map[2][last_map_index] == state::EMPTY)
		{
			player ? map[2][last_map_index] = state::CIRCLE : map[2][last_map_index] = state::CROSS;
			player ? draw_circle({ 2, last_map_index }) : draw_cross({ 2, last_map_index });
			player = !player;
			check_win();
		}
		break;
	case FOURTH_BUTTON_DOWN:
		clear_display();
		init_game();
		break;
	case THIRD_BUTTON_DOWN | FOURTH_BUTTON_DOWN:
		game_state = false;
		break;
	}

	last_buttons_check = clock();
}

/* Проверка резистора */
void check_resistor()
{
	if (clock() - last_resistor_check < 150)
		return;

	unsigned char resistor_value = get_resistor_value();
	if (resistor_value != last_resistor_value)
	{
		unsigned char new_index;

		if (resistor_value < 25)
			new_index = 2;
		else if (resistor_value < 100)
			new_index = 1;
		else
			new_index = 0;

		draw_arrow(new_index);
		last_resistor_value = resistor_value;
	}

	last_resistor_check = clock();
}

/* Главный игровой цикл */
void start_game()
{
	while (game_state)
	{
		if (arrowView)
			check_resistor();

		check_buttons();
	}
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	init_shield();
	srand(time(NULL));

	init_game();
	start_game();

	close_shield();
	return 0;
}