#include <list>
#include <queue>
#include <ctime>
#include <iostream>
#include <windows.h>

#include "spaceshield.h"

#pragma warning (disable: 26812) // ���������� �������������� ������������� enum class ������ enum
#pragma warning (disable: 6385) // ���������� �������������� ���������� ������ �� ������� �������
#pragma warning (disable: 6386) // ���������� �������������� ���������� ������������ ������

enum states // ��������� �������� �� ������
{
	EMPTY,
	BLOCK,
	FIGURE
};
states map[16][32] = { EMPTY }; // ������������ ��������� �� ������

enum shapes // ��� ��������� ����� �������� ������
{
	SHAPE_O = 0,
	SHAPE_I = 1,
	SHAPE_J = 2,
	SHAPE_L = 3,
	SHAPE_Z = 4,
	SHAPE_T = 5,
	SHAPE_S = 6
};

enum directions // ��� ��������� �������� �������� ������
{
	DIRECTION_0,
	DIRECTION_1,
	DIRECTION_2,
	DIRECTION_3
};

bool game_state = true; // ��������� ����

struct figure
{
	shapes shape;
	directions direction;
	bool is_falling; // ������ �� ������ 
	int x, y; // ���������� ������
} figure;

clock_t last_figure_fall = clock(), // �����, ����� ���� ���������� ������ ���� � ������� ���
last_resistor_check = clock(), // ����� ��������� �������� ���������
last_button_check = clock(); // ����� ��������� �������� ������

/* ���������� ����������� �� ������� */
void draw()
{
	for (size_t x = 0; x < 16; x++)
		for (size_t y = 0; y < 32; y++)
			for (size_t i = 0; i < 4; i++)
				for (size_t j = 0; j < 4; j++)
					draw_pixel({ (unsigned char)(y * 4 + j), (unsigned char)(x * 4 + i), (map[x][y] == EMPTY) ? (unsigned char)0 : (unsigned char)1 });
}

/* ����������� ������ ������ */
void move_shape_right()
{
	for (int x = 0; x < 16; x++) // �������� �����������
		for (int y = 0; y < 32; y++)
			if (map[x][y] == FIGURE && (x - 1 < 0 || map[x - 1][y] == BLOCK))
				return;

	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 32; y++)
			if (map[x][y] == FIGURE)
			{
				map[x - 1][y] = FIGURE;
				map[x][y] = EMPTY;
			}

	figure.x--;
}

/* ����������� ������ ����� */
void move_shape_left()
{
	for (int x = 15; x >= 0; x--) // �������� �����������
		for (int y = 31; y >= 0; y--)
			if (map[x][y] == FIGURE && (x + 1 == 16 || map[x + 1][y] == BLOCK))
				return;

	for (int x = 15; x >= 0; x--)
		for (int y = 31; y >= 0; y--)
			if (map[x][y] == FIGURE)
			{
				map[x + 1][y] = FIGURE;
				map[x][y] = EMPTY;
			}

	figure.x++;
}

/* �������� ��������� */
void check_resistor()
{
	if (!figure.is_falling)
		return;

	if (clock() - last_resistor_check < 500) // �������� ��������� ����� ������������ ���������� �������
		return;
	last_resistor_check = clock();

	unsigned char resistor_value = get_resistor_value();

	/* ����������� ������ ������ ��� �����*/
	if (resistor_value == 240)
		move_shape_right();
	else if (resistor_value == 0)
		move_shape_left();
}

/* ������� �������� ������ � ������ */
void remove_figure()
{
	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 32; y++)
			if (map[x][y] == FIGURE)
				map[x][y] = EMPTY;
}

/* ������� ������ � ����� I */
void flip_i()
{
	if (figure.direction == DIRECTION_0)
	{
		bool is_right_ban = (figure.x < 3),
			is_left_ban = (figure.x > 12);

		for (size_t x = 0; x < 4; x++) // ��������� ����������� ������ � �����
		{
			if (!is_left_ban && map[figure.x + x][figure.y] == BLOCK)
			{
				is_left_ban = true;

				if (is_right_ban)
					break;
			}
			
			if (!is_right_ban && map[figure.x - x][figure.y] == BLOCK)
			{
				is_right_ban = true;

				if (is_left_ban)
					break;
			}
		}

		if (!is_left_ban) // ������������� ������ �����, ���� ��������
		{
			figure.direction = DIRECTION_1;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x + x][figure.y] = FIGURE;

			return;
		}

		if (!is_right_ban) // ������������� ������ ������, ���� ��������
		{
			figure.direction = DIRECTION_1;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x - x][figure.y] = FIGURE;

			return;
		}
	}
	else
	{
		figure.direction = DIRECTION_0;

		remove_figure();

		for (size_t y = 0; y < 4; y++)
			map[figure.x][figure.y - y] = FIGURE;
	}
}

/* ������� ������ � ����� J */
void flip_j()
{
	if (figure.direction == DIRECTION_0)
	{
		bool is_left_ban = (figure.x > 12 || map[figure.x + 3][figure.y - 1] != EMPTY),
			is_right_ban = (figure.x < 3);

		for (size_t x = 0; x < 4; x++) // ��������� ����������� ������ � �����
		{
			if (!is_left_ban && map[figure.x + x][figure.y] == BLOCK)
			{
				is_left_ban = true;

				if (is_right_ban)
					break;
			}

			if (!is_right_ban && map[figure.x - x][figure.y] == BLOCK)
			{
				is_right_ban = true;

				if (is_left_ban)
					break;
			}
		}

		if (!is_left_ban) // ������������� ������ �����, ���� ��������
		{
			figure.direction = DIRECTION_1;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x + x][figure.y] = FIGURE;

			map[figure.x + 3][figure.y - 1] = FIGURE;

			return;
		}

		if (!is_right_ban) // ������������� ������ ������, ���� ��������
		{
			figure.direction = DIRECTION_1;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x - x][figure.y] = FIGURE;

			map[figure.x][figure.y - 1] = FIGURE;

			return;
		}
	}
	else if (figure.direction == DIRECTION_1)
	{
		if (figure.x == 0) // ������ ��������� � ������� ����
			figure.x += 3;

		if (map[figure.x - 1][figure.y - 3] != EMPTY)
			return;

		figure.direction = DIRECTION_2;

		remove_figure();

		for (size_t y = 0; y < 4; y++)
			map[figure.x][figure.y - y] = FIGURE;

		map[figure.x - 1][figure.y - 3] = FIGURE;
	}
	else if (figure.direction == DIRECTION_2)
	{
		bool is_left_ban = (figure.x > 12),
			is_right_ban = (figure.x < 3 || figure.x >= 3 && map[figure.x - 3][figure.y] != EMPTY);

		for (size_t x = 0; x < 4; x++) // ��������� ����������� ������ � �����
		{
			if (!is_left_ban && map[figure.x + x][figure.y - 1] == BLOCK)
			{
				is_left_ban = true;

				if (is_right_ban)
					break;
			}

			if (!is_right_ban && map[figure.x - x][figure.y - 1] == BLOCK)
			{
				is_right_ban = true;

				if (is_left_ban)
					break;
			}
		}

		if (!is_left_ban) // ������������� ������ �����, ���� ��������
		{
			figure.direction = DIRECTION_3;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x + x][figure.y - 1] = FIGURE;

			map[figure.x][figure.y] = FIGURE;

			return;
		}

		if (!is_right_ban) // ������������� ������ ������, ���� ��������
		{
			figure.direction = DIRECTION_3;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x - x][figure.y - 1] = FIGURE;

			figure.x -= 3;
			map[figure.x][figure.y] = FIGURE;

			return;
		}
	}
	else
	{
		if (figure.x == 0) // ������ ��������� � ������� ����
			figure.x += 1;

		figure.direction = DIRECTION_0;

		remove_figure();

		for (size_t y = 0; y < 4; y++)
			map[figure.x][figure.y - y] = FIGURE;

		map[figure.x + 1][figure.y] = FIGURE;
	}
}

/* ������� ������ � ����� L */
void flip_l()
{
	if (figure.direction == DIRECTION_0)
	{
		bool is_left_ban = (figure.x > 12),
			is_right_ban = (figure.x < 3 || map[figure.x - 3][figure.y - 1] != EMPTY);

		for (size_t x = 0; x < 4; x++) // ��������� ����������� ������ � �����
		{
			if (!is_left_ban && map[figure.x + x][figure.y] == BLOCK)
			{
				is_left_ban = true;

				if (is_right_ban)
					break;
			}

			if (!is_right_ban && map[figure.x - x][figure.y] == BLOCK)
			{
				is_right_ban = true;

				if (is_left_ban)
					break;
			}
		}

		if (!is_left_ban) // ������������� ������ �����, ���� ��������
		{
			figure.direction = DIRECTION_1;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x + x][figure.y] = FIGURE;

			map[figure.x][figure.y - 1] = FIGURE;

			return;
		}

		if (!is_right_ban) // ������������� ������ ������, ���� ��������
		{
			figure.direction = DIRECTION_1;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x - x][figure.y] = FIGURE;

			map[figure.x - 3][figure.y - 1] = FIGURE;

			return;
		}
	}
	else if (figure.direction == DIRECTION_1)
	{
		if (figure.x == 15) // ������ ��������� � ������ ����
			figure.x -= 1;

		if (map[figure.x + 1][figure.y - 3] != EMPTY)
			return;

		figure.direction = DIRECTION_2;

		remove_figure();

		for (size_t y = 0; y < 4; y++)
			map[figure.x][figure.y - y] = FIGURE;

		map[figure.x + 1][figure.y - 3] = FIGURE;
	}
	else if (figure.direction == DIRECTION_2)
	{
		bool is_left_ban = (figure.x > 12 || map[figure.x + 3][figure.y] != EMPTY),
			is_right_ban = (figure.x < 3);

		for (size_t x = 0; x < 4; x++) // ��������� ����������� ������ � �����
		{
			if (!is_left_ban && map[figure.x + x][figure.y - 1] == BLOCK)
			{
				is_left_ban = true;

				if (is_right_ban)
					break;
			}

			if (!is_right_ban && map[figure.x - x][figure.y - 1] == BLOCK)
			{
				is_right_ban = true;

				if (is_left_ban)
					break;
			}
		}

		if (!is_left_ban) // ������������� ������ �����, ���� ��������
		{
			figure.direction = DIRECTION_3;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x + x][figure.y - 1] = FIGURE;

			figure.x += 3;
			map[figure.x][figure.y] = FIGURE;

			return;
		}

		if (!is_right_ban) // ������������� ������ ������, ���� ��������
		{
			figure.direction = DIRECTION_3;

			remove_figure();

			for (size_t x = 0; x < 4; x++)
				map[figure.x - x][figure.y - 1] = FIGURE;

			map[figure.x][figure.y] = FIGURE;

			return;
		}
	}
	else
	{
		if (figure.x == 0) // ������ ��������� � ������� ����
			figure.x += 1;

		figure.direction = DIRECTION_0;

		remove_figure();

		for (size_t y = 0; y < 4; y++)
			map[figure.x][figure.y - y] = FIGURE;

		map[figure.x - 1][figure.y] = FIGURE;
	}
}

/* ������� ������ � ����� Z */
void flip_z()
{
	if (figure.direction == DIRECTION_0)
	{
		figure.direction = DIRECTION_1;

		if (figure.x == 0) // ������ ��������� � ������� ����
			figure.x++;

		remove_figure();

		map[figure.x][figure.y] = FIGURE;
		map[figure.x][figure.y - 1] = FIGURE;
		map[figure.x - 1][figure.y - 1] = FIGURE;
		map[figure.x - 1][figure.y - 2] = FIGURE;
	}
	else
	{
		if (figure.x == 15) // ������ ��������� � ������ ����
			figure.x--;

		if (map[figure.x - 1][figure.y] == BLOCK || map[figure.x + 1][figure.y - 1] == BLOCK) // ��������� �����������
			return;

		figure.direction = DIRECTION_0;

		remove_figure();

		map[figure.x][figure.y] = FIGURE;
		map[figure.x][figure.y - 1] = FIGURE;
		map[figure.x - 1][figure.y] = FIGURE;
		map[figure.x + 1][figure.y - 1] = FIGURE;
	}
}

/* ������� ������ � ����� S */
void flip_s()
{
	if (figure.direction == DIRECTION_0)
	{
		figure.direction = DIRECTION_1;

		remove_figure();

		map[figure.x][figure.y] = FIGURE;
		map[figure.x][figure.y - 1] = FIGURE;
		map[figure.x + 1][figure.y - 1] = FIGURE;
		map[figure.x + 1][figure.y - 2] = FIGURE;
	}
	else
	{
		if (figure.x == 0) // ������ ��������� � ������� ����
			figure.x++;

		if (map[figure.x + 1][figure.y] == BLOCK || map[figure.x - 1][figure.y - 1] == BLOCK) // ��������� �����������
			return;

		figure.direction = DIRECTION_0;

		remove_figure();

		map[figure.x][figure.y] = FIGURE;
		map[figure.x][figure.y - 1] = FIGURE;
		map[figure.x + 1][figure.y] = FIGURE;
		map[figure.x - 1][figure.y - 1] = FIGURE;
	}
}

/* ������� ������ � ����� T */
void flip_t()
{
	if (figure.direction == DIRECTION_0)
	{
		figure.direction = DIRECTION_1;

		map[figure.x - 1][figure.y - 1] = EMPTY;
		map[figure.x][figure.y - 2] = FIGURE;
	}
	else if (figure.direction == DIRECTION_1)
	{
		if (map[figure.x - 1][figure.y] == BLOCK || map[figure.x + 1][figure.y] == BLOCK) // ��������� �����������
			return;

		if (figure.x == 0) // ������ ��������� � ������� ����
			figure.x++;

		figure.direction = DIRECTION_2;

		remove_figure();

		map[figure.x][figure.y] = FIGURE;
		map[figure.x][figure.y - 1] = FIGURE;
		map[figure.x - 1][figure.y] = FIGURE;
		map[figure.x + 1][figure.y] = FIGURE;
	}
	else if (figure.direction == DIRECTION_2)
	{
		figure.direction = DIRECTION_3;

		map[figure.x + 1][figure.y] = EMPTY;
		map[figure.x - 1][figure.y] = EMPTY;

		map[figure.x][figure.y - 2] = FIGURE;
		map[figure.x - 1][figure.y - 1] = FIGURE;
	}
	else if (figure.direction == DIRECTION_3)
	{
		if (figure.x == 15) // ������ ��������� � ������ ����
			figure.x--;

		figure.direction = DIRECTION_0;

		remove_figure();

		map[figure.x][figure.y] = FIGURE;
		map[figure.x][figure.y - 1] = FIGURE;
		map[figure.x - 1][figure.y - 1] = FIGURE;
		map[figure.x + 1][figure.y - 1] = FIGURE;
	}
}

/* ������� ������ */
void flip_shape()
{
	switch (figure.shape)
	{
	case SHAPE_O:
		return; // � ���������� ���� ��������� 
	case SHAPE_I:
		flip_i();
		break;
	case SHAPE_J:
		flip_j();
		break;
	case SHAPE_L:
		flip_l();
		break;
	case SHAPE_Z:
		flip_z();
		break;
	case SHAPE_S:
		flip_s();
		break;
	case SHAPE_T:
		flip_t();
		break;
	}
}

/* ��������� ���������� ������ */
void drop_figure()
{
	while (figure.is_falling) // ���� �� �������� ������ �� ������?
	{
		for (int x = 15; x >= 0; x--) // �������� �� ������������� � �����
			for (int y = 31; y >= 0; y--)
				if (map[x][y] == FIGURE && (y + 1 == 31 || map[x][y + 1] == BLOCK))
				{
					figure.is_falling = false;

					for (int x = 15; x >= 0; x--)
						for (int y = 31; y >= 0; y--)
							if (map[x][y] == FIGURE)
								map[x][y] = BLOCK;

					return;
				}

		for (int x = 15; x >= 0; x--)
			for (int y = 31; y >= 0; y--)
				if (map[x][y] == FIGURE)
				{
					map[x][y + 1] = FIGURE;
					map[x][y] = EMPTY;
				}
		figure.y++;
	}
}

/* �������� ������� ������ */
void check_buttons()
{
	if (clock() - last_button_check < 1000)  // �������� ������ ����� ������������ ���������� �������
		return;
	last_button_check = clock();

	unsigned char button_state = get_buttons_state();

	if (button_state == FOURTH_BUTTON_DOWN)
		flip_shape();

	if (button_state == THIRD_BUTTON_DOWN)
		drop_figure();
}

/* ��������� ����� ������ */
void spawn_figure()
{
	figure.shape = (shapes)(rand() % 7);
	figure.direction = DIRECTION_0;	

	if (map[7][3] == BLOCK) // ���������
	{
		game_state = false;
		
		return;
	}

	figure.x = 7;
	figure.y = 3;

	switch (figure.shape)
	{
	case SHAPE_O:
		map[7][2] = FIGURE;
		map[8][2] = FIGURE;
		map[7][3] = FIGURE;
		map[8][3] = FIGURE;
		break;
	case SHAPE_I:
		map[7][0] = FIGURE;
		map[7][1] = FIGURE;
		map[7][2] = FIGURE;
		map[7][3] = FIGURE;
		break;
	case SHAPE_J:
		map[7][0] = FIGURE;
		map[7][1] = FIGURE;
		map[7][2] = FIGURE;
		map[7][3] = FIGURE;
		map[8][3] = FIGURE;
		break;
	case SHAPE_L:
		map[8][0] = FIGURE;
		map[8][1] = FIGURE;
		map[8][2] = FIGURE;
		map[8][3] = FIGURE;
		map[7][3] = FIGURE;
		break;
	case SHAPE_Z:
		map[8][2] = FIGURE;
		map[9][2] = FIGURE;
		map[8][3] = FIGURE;
		map[7][3] = FIGURE;
		break;
	case SHAPE_S:
		map[8][2] = FIGURE;
		map[7][2] = FIGURE;
		map[8][3] = FIGURE;
		map[9][3] = FIGURE;
		break;
	case SHAPE_T:
		map[6][2] = FIGURE;
		map[7][2] = FIGURE;
		map[8][2] = FIGURE;
		map[7][3] = FIGURE;
		break;
	}

	figure.is_falling = true;
}

/* �������� �������� ������ */
void check_figure()
{
	if (clock() - last_figure_fall < 500)
		return;
	last_figure_fall = clock();

	if (!figure.is_falling) // ���� �� �������� ������ �� ������?
	{
		spawn_figure();

		return;
	}

	for (int x = 15; x >= 0; x--) // �������� �� ������������� � �����
		for (int y = 31; y >= 0; y--)
			if (map[x][y] == FIGURE && (y + 1 == 31 || map[x][y + 1] == BLOCK))
			{
				figure.is_falling = false;

				for (int x = 15; x >= 0; x--)
					for (int y = 31; y >= 0; y--)
						if (map[x][y] == FIGURE)
							map[x][y] = BLOCK;

				return;
			}

	for (int x = 15; x >= 0; x--)
		for (int y = 31; y >= 0; y--)
			if (map[x][y] == FIGURE)
			{
				map[x][y + 1] = FIGURE;
				map[x][y] = EMPTY;
			}
	figure.y++;
}

/* �������� ����� ����� */
void check_lines()
{
	for (int y = 0; y < 32; y++)
	{
		int blocks = 0;

		for (int x = 0; x < 16; x++)
			if (map[x][y] == BLOCK)
				blocks++;
		
		if (blocks == 16) // ��� ����� ���������
		{
			for (int x = 0; x < 16; x++)
				map[x][y] = EMPTY;

			for (int x = 15; x >= 0; x--)
				for (int j = y; j >= 0; j--)
					if (map[x][j] == BLOCK)
					{
						map[x][j] = EMPTY;
						map[x][j + 1] = BLOCK;
					}
		}
	}
}

/* �������� ������� ������� */
void update_events()
{
	check_buttons();
	check_resistor();
	check_figure();
	check_lines();
}

/* ������� ���� ���� */
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