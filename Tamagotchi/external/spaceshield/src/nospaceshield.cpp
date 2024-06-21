#include "spaceshield.h"

#include <iostream>

// ImageMagick dependency for jpg
#include "CImg.h"

#define NO_SHIELD

#include <ctime>
cimg_library::CImgDisplay display(256, 128, "Shield display");

time_t button_time,
    resistor_time;

led_color old_color = {0, 0, 0};

unsigned char resistor_value = 0;

void init_shield()
{
    button_time = resistor_time = time(NULL);
}

void close_shield()
{
    return;
}

void set_led_color(led_color color)
{
    if (old_color.r == color.r && old_color.g == color.g && old_color.b == color.b)
        return;

    old_color = color;
    printf("LED color: r - %d, g - %d, b - %d\n", color.r, color.g, color.b);
}

unsigned char get_resistor_value()
{
    if (time(NULL) - resistor_time > 30)
    {
        unsigned char tmp;

        std::cout << "Хотите ли вы поменять значение резистора? (1 - да, 0 - нет): ";
        std::cin >> tmp;

        if (tmp)
        {
            std::cout << "Введите значение резистора (0 - 255): ";
            std::cin >> tmp;

            resistor_value = tmp;
        }

        resistor_time = time(NULL);
    }

    return resistor_value;
}

void clear_display()
{
    return;
}

void draw_pixel(display_point point)
{
    return;
}

void draw_image(const char *path)
{
    cimg_library::CImg<unsigned char> img(path);
    img.resize(128, 64, 1, 1, 5);

    img.display(display);
}

unsigned char get_buttons_state()
{
    if (time(NULL) - button_time > 10)
    {
        int choice = 0;

        std::cout << "При желании введите номер кнопки (всего их 4): ";
        std::cin >> choice;

        button_time = time(NULL);

        switch (choice)
        {
        case 1:
            return FIRST_BUTTON_DOWN;
        case 2:
            return SECOND_BUTTON_DOWN;
        case 3:
            return THIRD_BUTTON_DOWN;
        case 4:
            return FOURTH_BUTTON_DOWN;
        default:
            return 0;
        }
    }

    return 0;
}
