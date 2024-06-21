#include <list>
#include <queue>
#include <ctime>
#include <iostream>
#include <windows.h>

#include "spaceshield.h"

#pragma warning (disable: 26812) // отключение предупреждения использования enum class вместо enum
#pragma warning (disable: 6385) // отключение предупреждения возможного выхода за границы массива
#pragma warning (disable: 6386) // отключение предупреждения возможного переполнения буфера

enum states // Возможные состояния игрового поля
{
	EMPTY,
	BIRD,
	PIPE
};
states display[128][64] = { EMPTY };

bool game_state = true; // состояние игры

int bird_y = 20, // координаты птички
	pipes_interval = 0; // интервал между трубами

clock_t last_button_check = clock(), // время последней проверки кнопок
		last_bird_check = clock(), // время последней проверки птички
		last_pipes_check = clock(); // время последней проверки труб

/* Убирает птичку с дисплея */
void remove_bird()
{
	for (size_t x = 0; x < 23; x++)
		for (size_t y = 0; y < 64; y++)
			if (display[x][y] == BIRD)
				display[x][y] = EMPTY;
}

/* Рисование птицы на дисплее */
void draw_bird()
{
	remove_bird();

	for (size_t i = 0; i < 4; i++)
		display[13 + i][bird_y + 11] = BIRD;

	for (size_t i = 0; i < 5; i++)
		display[17 + i][bird_y + 10] = BIRD;
	for (size_t i = 0; i < 2; i++)
		display[11 + i][bird_y + 10] = BIRD;

	display[21][bird_y + 9] = BIRD;
	display[16][bird_y + 9] = BIRD;
	display[10][bird_y + 9] = BIRD;

	for (size_t i = 0; i < 3; i++)
		display[7 + i][bird_y + 8] = BIRD;
	display[15][bird_y + 8] = BIRD;
	for (size_t i = 0; i < 5; i++)
		display[17 + i][bird_y + 8] = BIRD;

	display[22][bird_y + 7] = BIRD;
	display[16][bird_y + 7] = BIRD;
	display[10][bird_y + 7] = BIRD;
	display[6][bird_y + 7] = BIRD;

	for (size_t i = 0; i < 5; i++)
		display[17 + i][bird_y + 6] = BIRD;
	display[11][bird_y + 6] = BIRD;
	display[5][bird_y + 6] = BIRD;
	
	display[5][bird_y + 5] = BIRD;
	display[11][bird_y + 5] = BIRD;
	display[16][bird_y + 5] = BIRD;
	display[20][bird_y + 5] = BIRD;

	display[5][bird_y + 4] = BIRD;
	display[10][bird_y + 4] = BIRD;
	display[15][bird_y + 4] = BIRD;
	display[18][bird_y + 4] = BIRD;
	display[20][bird_y + 4] = BIRD;

	for (size_t i = 0; i < 4; i++)
		display[6 + i][bird_y + 3] = BIRD;
	display[15][bird_y + 3] = BIRD;
	display[18][bird_y + 3] = BIRD;
	display[20][bird_y + 3] = BIRD;

	display[9][bird_y + 2] = BIRD;
	display[15][bird_y + 2] = BIRD;
	display[19][bird_y + 2] = BIRD;

	display[11][bird_y + 1] = BIRD;
	display[10][bird_y + 1] = BIRD;
	display[16][bird_y + 1] = BIRD;
	display[18][bird_y + 1] = BIRD;

	for (size_t i = 0; i < 6; i++)
		display[12 + i][bird_y] = BIRD;
}

/* Обновление изображения на дисплее */
void draw()
{
	draw_bird();

	for (unsigned char x = 0; x < 128; x++)
		for (unsigned char y = 0; y < 64; y++)
			draw_pixel({ x, y, (display[x][y] == EMPTY) ? (unsigned char)0 : (unsigned char)1 });
}

/* Проверка нажатия кнопок */
void check_buttons()
{
	if (clock() - last_button_check < 1000)  // Проверка кнопки через определенный промежуток времени
		return;
	last_button_check = clock();

	unsigned char button_state = get_buttons_state();

	if (button_state == FIRST_BUTTON_DOWN)
	{
		if (bird_y < 8) // Чтобы птичка не улетела за потолок
			return;

		bird_y -= 8;
	}
}

/* Проверка птички */
void check_bird()
{
	if (clock() - last_bird_check < 200) // Проверка птички в определенный промежуток времени
		return;
	last_bird_check = clock();

	bird_y++;

	if (bird_y == 52) // Птичка упала на пол
		game_state = false;
}

/* Появление новой трубы */
void spawn_pipe()
{
	int hole = rand() % 30 + 5; // Случайно выбираем расположение отверстия

	for (size_t y = 0; y < 64; y++) // Рисуем вертикальные линии
	{
		display[127][y] = PIPE;
		display[111][y] = PIPE;
	}

	for (size_t y = 0; y < 25; y++) // Создаем отверстие в трубе
	{
		display[127][hole + y] = EMPTY;
		display[111][hole + y] = EMPTY;
	}

	for (size_t x = 0; x < 17; x++) // Рисуем горизонтальные линии
	{
		display[127 - x][hole] = PIPE;
		display[127 - x][hole + 25] = PIPE;
	}

	pipes_interval = 80;
}

/* Проверка труб */
void check_pipes()
{
	if (clock() - last_pipes_check < 100) // Проверка труб в определенный промежуток времени
		return;
	last_pipes_check = clock();

	if (!pipes_interval) // Нужно ли создавать новую струбу?
	{
		spawn_pipe();

		return;
	}

	for (size_t x = 0; x < 128; x++) // Сдвигаем трубы влево
		for (size_t y = 0; y < 64; y++)
			if (display[x][y] == PIPE)
			{
				display[x][y] = EMPTY;

				if (display[x - 1][y] == BIRD) // Проверяем соприкосновение с птицей
				{
					game_state = false;

					std::cout << "bird" << std::endl;

					return;
				}

				if (x) // Труба у левой границы экрана
					display[x - 1][y] = PIPE;
			}

	pipes_interval--;
}

/* Проверка игровых событий */
void update_events()
{
	check_buttons();
	check_bird();
	check_pipes();
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

	start_game();

	close_shield();
	return 0;
}