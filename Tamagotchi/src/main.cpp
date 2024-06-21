#include <ctime>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "spaceshield.h"

struct game_stats
{
	unsigned char happy;
	unsigned char hungry;
	unsigned char sanity;
	unsigned char sleep;
} game_stats, old_stats;

bool game_state;

enum game_scenes
{
	SCENE_HAPPY,
	SCENE_NORMAL,
	SCENE_SAD,
	SCENE_EAT,
	SCENE_BATH,
	SCENE_FUN,
	SCENE_SLEEP,
	SCENE_DEATH,
	SCENE_NO
} game_scene;
char *scene_image = "img/error.jpg";
time_t scene_time,
	hunger_time,
	happy_time,
	sanity_time,
	sleep_time;

led_color color_status;

void death_event()
{
	for (size_t i = 0; i < 66; i++)
	{
		std::string path = "img/death/";
		path += std::to_string(i);
		path += ".gif";

		draw_image(path.c_str());
	}

	game_state = false;
}

void draw_scene()
{
	if (game_scene != SCENE_DEATH)
		draw_image(scene_image);
}

void change_scene(game_scenes scene)
{
	if (game_scene == scene)
		return;

	game_scene = scene;
	scene_time = time(NULL);

	switch (game_scene)
	{
	case SCENE_HAPPY:
		switch (rand() % 5)
		{
		case 0:
			scene_image = "img/happy1.jpg";
			break;
		case 1:
			scene_image = "img/happy2.jpg";
			break;
		case 2:
			scene_image = "img/happy3.jpg";
			break;
		case 3:
			scene_image = "img/happy4.jpg";
			break;
		case 4:
			scene_image = "img/happy5.jpg";
			break;
		default:
			scene_image = "img/error.jpg";
			break;
		}
		break;
	case SCENE_NORMAL:
		switch (rand() % 11)
		{
		case 0:
			scene_image = "img/idle1.jpg";
			break;
		case 1:
			scene_image = "img/idle2.jpg";
			break;
		case 2:
			scene_image = "img/idle3.jpg";
			break;
		case 3:
			scene_image = "img/idle4.jpg";
			break;
		case 4:
			scene_image = "img/idle4.jpg";
			break;
		case 5:
			scene_image = "img/idle5.jpg";
			break;
		case 6:
			scene_image = "img/idle6.jpg";
			break;
		case 7:
			scene_image = "img/idle7.jpg";
			break;
		case 8:
			scene_image = "img/idle8.jpg";
			break;
		case 9:
			scene_image = "img/idle9.jpg";
			break;
		case 10:
			scene_image = "img/idle10.jpg";
			break;
		default:
			scene_image = "img/error.jpg";
			break;
		}
		break;
	case SCENE_SAD:
		switch (rand() % 5)
		{
		case 0:
			scene_image = "img/sad1.jpg";
			break;
		case 1:
			scene_image = "img/sad2.jpg";
			break;
		case 2:
			scene_image = "img/sad3.jpg";
			break;
		case 3:
			scene_image = "img/sad4.jpg";
			break;
		case 4:
			scene_image = "img/sad5.jpg";
			break;
		default:
			scene_image = "img/error.jpg";
			break;
		}
		break;
	case SCENE_EAT:
		switch (rand() % 5)
		{
		case 0:
			scene_image = "img/eat1.jpg";
			break;
		case 1:
			scene_image = "img/eat2.jpg";
			break;
		case 2:
			scene_image = "img/eat3.jpg";
			break;
		case 3:
			scene_image = "img/eat4.jpg";
			break;
		case 4:
			scene_image = "img/eat5.jpg";
			break;
		default:
			scene_image = "img/error.jpg";
			break;
		}
		break;
	case SCENE_BATH:
		switch (rand() % 3)
		{
		case 0:
			scene_image = "img/bath1.jpg";
			break;
		case 1:
			scene_image = "img/bath2.jpg";
			break;
		case 2:
			scene_image = "img/bath3.jpg";
			break;
		default:
			scene_image = "img/error.jpg";
			break;
		}
		break;
	case SCENE_FUN:
		switch (rand() % 5)
		{
		case 0:
			scene_image = "img/fun1.jpg";
			break;
		case 1:
			scene_image = "img/fun2.jpg";
			break;
		case 2:
			scene_image = "img/fun3.jpg";
			break;
		case 3:
			scene_image = "img/fun4.jpg";
			break;
		case 4:
			scene_image = "img/fun5.jpg";
			break;
		default:
			scene_image = "img/error.jpg";
			break;
		}
		break;
	case SCENE_SLEEP:
		switch (rand() % 5)
		{
		case 0:
			scene_image = "img/sleep1.jpg";
			break;
		case 1:
			scene_image = "img/sleep2.jpg";
			break;
		case 2:
			scene_image = "img/sleep3.jpg";
			break;
		case 3:
			scene_image = "img/sleep4.jpg";
			break;
		case 4:
			scene_image = "img/sleep5.jpg";
			break;
		default:
			scene_image = "img/error.jpg";
			break;
		}
		break;
	case SCENE_NO:
		scene_image = "img/no.jpg";
		break;
	case SCENE_DEATH:
		death_event();
		break;
	}
}

void bad_apple()
{
	for (size_t i = 42; i <= 6439; i++)
	{
		std::string path = "img/apple/";
		path += std::to_string(i);
		path += ".png";

		draw_image(path.c_str());
	}
}

void check_buttons()
{
	unsigned char button_state;

	button_state = get_buttons_state();

	switch (button_state)
	{
	case FIRST_BUTTON_DOWN:
		change_scene(SCENE_EAT);
		break;
	case SECOND_BUTTON_DOWN:
		change_scene(SCENE_FUN);
		break;
	case THIRD_BUTTON_DOWN:
		change_scene(SCENE_BATH);
		break;
	case FOURTH_BUTTON_DOWN:
		change_scene(SCENE_SLEEP);
		break;
	case (THIRD_BUTTON_DOWN | FOURTH_BUTTON_DOWN): // Выход из игры
		game_state = false;
		break;
	case (FIRST_BUTTON_DOWN | SECOND_BUTTON_DOWN | THIRD_BUTTON_DOWN | FOURTH_BUTTON_DOWN):
		bad_apple();
		break;
	}
}

void check_events()
{
	if (game_scene != SCENE_NORMAL && game_scene != SCENE_HAPPY && game_scene != SCENE_SAD && time(NULL) - scene_time > 5)
	{
		switch (game_scene)
		{
		case SCENE_BATH:
			if (100 - game_stats.sanity >= 25)
				game_stats.sanity += 25;
			else
				game_stats.sanity = 100;
			break;
		case SCENE_EAT:
			if (100 - game_stats.hungry >= 20)
				game_stats.hungry += 20;
			else
				game_stats.hungry = 100;
			break;
		case SCENE_FUN:
			if (100 - game_stats.happy >= 30)
				game_stats.happy += 30;
			else
				game_stats.happy = 100;
			break;
		case SCENE_SLEEP:
			if (100 - game_stats.sleep >= 50)
				game_stats.sleep += 50;
			else
				game_stats.sleep = 100;
			break;
		}

		change_scene(SCENE_NORMAL);
	}

	time_t cur_time = time(NULL);
	if (cur_time - hunger_time > get_resistor_value() + 1)
	{
		if (game_stats.hungry != 0)
			(game_scene == SCENE_SLEEP || game_scene == SCENE_FUN) ? (game_stats.hungry -= 5) : (game_stats.hungry -= 1);
		else
			change_scene(SCENE_DEATH);
		hunger_time = cur_time;
	}

	if (cur_time - happy_time > get_resistor_value() + 1 + (game_stats.sanity / 20) + (game_stats.sleep / 50) + (game_stats.hungry / 25))
	{
		if (game_stats.happy != 0)
			game_stats.happy -= 1;
		happy_time = cur_time;
	}

	if (cur_time - sanity_time > (get_resistor_value() + 1) * 5)
	{
		if (game_stats.sanity != 0)
			(game_scene == SCENE_EAT || game_scene == SCENE_FUN) ? (game_stats.sanity -= 5) : (game_stats.sanity -= 1);
		sanity_time = cur_time;
	}

	if (cur_time - sleep_time > (get_resistor_value() + 1) * 2)
	{
		if (game_stats.sleep != 0)
			(game_scene == SCENE_FUN || game_scene == SCENE_BATH) ? (game_stats.sleep -= 5) : (game_stats.sleep -= 1);
		sleep_time = cur_time;
	}

	int total = game_stats.happy + game_stats.hungry + game_stats.sanity + game_stats.sleep;
	if (game_scene == SCENE_NORMAL && total > 320)
		change_scene(SCENE_HAPPY);
	else if (game_scene == SCENE_NORMAL && total < 130)
		change_scene(SCENE_SAD);
	else if ((game_scene == SCENE_HAPPY || game_scene == SCENE_SAD) && total < 320 && total > 130)
		change_scene(SCENE_NORMAL);
}

void draw_stats()
{
	if (old_stats.happy == game_stats.happy && old_stats.hungry == game_stats.hungry && old_stats.sanity == game_stats.sanity && old_stats.sleep == game_stats.sleep)
		return;

	system("cls");

	printf("Sleep:     ");
	for (size_t i = 0; i < game_stats.sleep; i++)
		printf("█");
	printf("\n\n");

	printf("Hunger:    ");
	for (size_t i = 0; i < game_stats.hungry; i++)
		printf("█");
	printf("\n\n");

	printf("Sanity:    ");
	for (size_t i = 0; i < game_stats.sanity; i++)
		printf("█");
	printf("\n\n");

	printf("Happiness: ");
	for (size_t i = 0; i < game_stats.happy; i++)
		printf("█");
	printf("\n\n");

	old_stats = game_stats;
}

void update_led()
{
	unsigned char total = game_stats.hungry;
	color_status.b = 0;

	if (total < 30)
	{
		color_status.r = total;
		color_status.g = 0;
		set_led_color(color_status);
		return;
	}
	else
	{
		total -= 30;
		color_status.r = 30;
	}

	if (total < 30)
	{
		color_status.g = total;
		set_led_color(color_status);
		return;
	}
	else
	{
		total -= 30;
		color_status.g = 30;
	}

	if (total < 30)
		color_status.r -= total;
	else
		color_status.r = 0;

	set_led_color(color_status);
}

void start_game()
{
	while (game_state)
	{
		check_buttons();
		check_events();

		draw_scene();

		draw_stats();
		update_led();
	}
}

void init_game()
{
	game_state = true;

	game_stats = {50, 50, 50, 50};
	old_stats = {0, 0, 0, 0};

	hunger_time = sanity_time = happy_time = sleep_time = time(NULL);

	change_scene(SCENE_NORMAL);
}

int main()
{
	SetConsoleOutputCP(65001);
	init_shield();
	srand(time(NULL));

	// init_game();
	// start_game();
	bad_apple();

	close_shield();
	return 0;
}
