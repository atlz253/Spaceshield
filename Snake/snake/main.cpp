#include <list>
#include <queue>
#include <ctime>
#include <iostream>
#include <windows.h>

#include "spaceshield.h"

#pragma warning (disable: 26812) // отключаем предупреждение C26812 (использзовать enum class вместо обычных enum)


enum states // возможные элементы на экране
{
	EMPTY,
	SNAKE,
	FOOD
};
states map[32][16] = { EMPTY }; // расположение элементов на экране

enum direction // возможные направления змейки
{
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT
};

struct point // точка на экране
{
	unsigned char x;
	unsigned char y;
};

struct snake
{
	enum direction direction = MOVE_RIGHT; // направление змейки
	std::list<point> elements = std::list<point>(); // координаты всех частей змейки
	std::list<point>::iterator iterator = std::list<point>::iterator(); // итератор для перемещения по списку
} snake;

clock_t last_move = clock(), // время последнего хода змейки
last_food = last_move; // время последнего появления пищи 

bool snake_new_direction_move = false, // прошла ли змейка в новом направлении хотя бы один раз?
game_state = true; // запущена ли игра?

/* Устанавливка стандартных значений змейки */
void init_snake()
{
	map[16][8] = SNAKE;
	snake.elements.push_back({ 16, 8 });

	map[15][8] = SNAKE;
	snake.elements.push_back({ 15, 8 });

	map[14][8] = SNAKE;
	snake.elements.push_back({ 14, 8 });
}

/* Обновление изображения на дисплее */
void draw()
{
	for (size_t x = 0; x < 32; x++)
		for (size_t y = 0; y < 16; y++)
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					draw_pixel({ (unsigned char)(x * 4 + i), (unsigned char)(y * 4 + j), (map[x][y] == EMPTY) ? (unsigned char)0 : (unsigned char)1 });
}

/* Проверка нажатия кнопок */
void check_buttons()
{
	if (!snake_new_direction_move)
		return;

	switch (get_buttons_state())
	{
	case FIRST_BUTTON_DOWN:
		switch (snake.direction)
		{
		case MOVE_UP:
			snake.direction = MOVE_LEFT;
			break;
		case MOVE_DOWN:
			snake.direction = MOVE_RIGHT;
			break;
		case MOVE_LEFT:
			snake.direction = MOVE_DOWN;
			break;
		case MOVE_RIGHT:
			snake.direction = MOVE_UP;
			break;
		}
		break;
	case FOURTH_BUTTON_DOWN:
		switch (snake.direction)
		{
		case MOVE_UP:
			snake.direction = MOVE_RIGHT;
			break;
		case MOVE_DOWN:
			snake.direction = MOVE_LEFT;
			break;
		case MOVE_LEFT:
			snake.direction = MOVE_UP;
			break;
		case MOVE_RIGHT:
			snake.direction = MOVE_DOWN;
			break;
		}
		break;
	}

	snake_new_direction_move = false;
}

/* Проверка необходимости перемещения змейки */
void check_snake_move()
{
	if (clock() - last_move < 300 + get_resistor_value() * 3)
		return;

	point prev_element = *snake.elements.begin(); // старые координаты предыдущего элемента
	
	snake.iterator = snake.elements.begin();

	/* Перемещение головы змейки */
	switch (snake.direction)
	{
	case MOVE_UP:
		std::cout << "head up" << std::endl;

		if (snake.iterator->y == 0) // проверка на границы экрана
		{
			game_state = false;
			return;
		}

		if (map[snake.iterator->x][snake.iterator->y - 1] == FOOD) // рост змейки
		{
			map[snake.iterator->x][snake.iterator->y - 1] = SNAKE;
			snake.elements.push_front({ snake.iterator->x, (unsigned char)(snake.iterator->y - 1) });
			return;
		}
		else if (map[snake.iterator->x][snake.iterator->y - 1] == SNAKE) // змейка врезалась в свое тело
		{
			game_state = false;
			return;
		}

		map[snake.iterator->x][snake.iterator->y - 1] = map[snake.iterator->x][snake.iterator->y];
		map[snake.iterator->x][snake.iterator->y] = EMPTY;
		snake.iterator->y -= 1;
		break;
	case MOVE_DOWN:
		std::cout << "head down" << std::endl;

		if (snake.iterator->y == 15) // проверка на границы экрана
		{
			game_state = false;
			return;
		}

		if (map[snake.iterator->x][snake.iterator->y + 1] == FOOD) // рост змейки
		{
			map[snake.iterator->x][snake.iterator->y + 1] = SNAKE;
			snake.elements.push_front({ snake.iterator->x, (unsigned char)(snake.iterator->y + 1) });
			return;
		}
		else if (map[snake.iterator->x][snake.iterator->y + 1] == SNAKE) // змейка врезалась в свое тело
		{
			game_state = false;
			return;
		}

		map[snake.iterator->x][snake.iterator->y + 1] = map[snake.iterator->x][snake.iterator->y];
		map[snake.iterator->x][snake.iterator->y] = EMPTY;
		snake.iterator->y += 1;
		break;
	case MOVE_LEFT:
		std::cout << "head left" << std::endl;

		if (snake.iterator->x == 0) // проверка на границы экрана
		{
			game_state = false;
			return;
		}

		if (map[snake.iterator->x - 1][snake.iterator->y] == FOOD) // рост змейки
		{
			map[snake.iterator->x - 1][snake.iterator->y] = SNAKE;
			snake.elements.push_front({ (unsigned char)(snake.iterator->x - 1), snake.iterator->y });
			return;
		}
		else if (map[snake.iterator->x - 1][snake.iterator->y] == SNAKE) // змейка врезалась в свое тело
		{
			game_state = false;
			return;
		}

		map[snake.iterator->x - 1][snake.iterator->y] = map[snake.iterator->x][snake.iterator->y];
		map[snake.iterator->x][snake.iterator->y] = EMPTY;
		snake.iterator->x -= 1;
		break;
	case MOVE_RIGHT:
		std::cout << "head right" << std::endl;

		if (snake.iterator->x == 31) // проверка на границы экрана
		{
			game_state = false;
			return;
		}

		if (map[snake.iterator->x + 1][snake.iterator->y] == FOOD) // рост змейки
		{
			map[snake.iterator->x + 1][snake.iterator->y] = SNAKE;
			snake.elements.push_front({ (unsigned char)(snake.iterator->x + 1), snake.iterator->y });
			return;
		}
		else if (map[snake.iterator->x + 1][snake.iterator->y] == SNAKE) // змейка врезалась в свое тело
		{
			game_state = false;
			return;
		}

		map[snake.iterator->x + 1][snake.iterator->y] = map[snake.iterator->x][snake.iterator->y];
		map[snake.iterator->x][snake.iterator->y] = EMPTY;
		snake.iterator->x += 1;
		break;
	}
	snake.iterator++;

	/* Разблокировка изменения направления */
	if (!snake_new_direction_move)
		snake_new_direction_move = true;

	/* Перемещение тела змейки */
	for (size_t i = 0; i < snake.elements.size() - 1; i++)
	{
		point tmp = *snake.iterator;

		map[snake.iterator->x][snake.iterator->y] = EMPTY;
		map[prev_element.x][prev_element.y] = SNAKE;

		snake.iterator->x = prev_element.x;
		snake.iterator->y = prev_element.y;

		prev_element = tmp;
		snake.iterator++;
	}

	last_move = clock();
}

void check_food()
{
	if (clock() - last_food < 5000)
		return;

	unsigned char x = rand() % 31, // Выбор случайной ячейки для размещения еды
		y = rand() % 15;

	if (map[x][y] == EMPTY)
	{
		map[x][y] = FOOD;
	}
	else // Не повезло, ячейка занята. Начинается обход в ширину для поиска свободной ячейки;
	{
		bool used[32][16] = { false };
		std::queue<point> queue;

		used[x][y] = true;
		queue.push({ x, y });

		while (!queue.empty())
		{
			point cell = queue.front();
			queue.pop();

			if (map[cell.x][cell.y] == EMPTY) // Нашли свободную ячейку
			{
				map[cell.x][cell.y] == FOOD;
				return;
			}

			if (cell.x != 0 && !used[x - 1][y])
			{
				used[x - 1][y] = true;
				queue.push({ (unsigned char)(cell.x - 1), cell.y });
			}

			if (cell.x != 31 && !used[x + 1][y])
			{
				used[x + 1][y] = true;
				queue.push({ (unsigned char)(cell.x + 1), cell.y });
			}

			if (cell.y != 0 && !used[x][y - 1])
			{
				used[x][y - 1] = true;
				queue.push({ cell.x, (unsigned char)(cell.y - 1) });
			}

			if (cell.y != 15 && !used[x][y + 1])
			{
				used[x][y + 1] = true;
				queue.push({ cell.x, (unsigned char)(cell.y + 1) });
			}
		}
	}


	last_food = clock();
}

/* Проверка игровых событий */
void update_events()
{
	check_buttons();
	check_snake_move();
	check_food();
}

/* Главный цикл игры */
void start_game()
{
	while (game_state)
	{
		update_events();
		draw();
	}
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	init_shield();
	srand(time(NULL));

	init_snake();
	start_game();

	close_shield();
	return 0;
}