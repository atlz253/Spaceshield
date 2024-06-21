// libSpaceshield by Fedor Alekseev
// 2022

#ifndef LIB_SPACESHIELD
#define LIB_SPACESHIELD

#include <iostream>

#define DllExport   __declspec( dllexport )

/*
    Инициализация библиотеки для корректной работы с устройством
*/
DllExport void init_shield();

/*
    Освобождение ресурсов, занятых библиотекой
*/
DllExport void close_shield();

#define LED_GREEN {0, 5, 0} // Зеленый цвет светодиода
#define LED_RED {5, 0, 0} // Красный цвет светодиода

/*
    Представление цвета светодиода в программе
*/
typedef DllExport struct led_color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} led_color;

/*
    Светодиод устанавливает переданный цвет
*/
DllExport void set_led_color(led_color color);


/*
    Получение значения переменного резистора
*/
DllExport unsigned char get_resistor_value();

/*
    Координаты пикселя на экране
*/
typedef DllExport struct display_point
{
    unsigned char x;
    unsigned char y;
} display_point;

/*
    Возможные цвета на дисплее
*/
DllExport enum display_color
{
    DISPLAY_COLOR_BLACK = 0x00,
    DISPLAY_COLOR_WHITE = 0x01
};

/*
    Закрашивает дисплей в переданный цвет
*/
DllExport void display_fill(display_color color);

/*
    Функция рисования пикселя на дисплее
*/
DllExport void draw_pixel(display_point point, display_color color);

/*
    Одновременное рисование нескольких пикселей на дисплее
*/
DllExport void draw_poly_pixel(int count, unsigned char *pixels, bool allow_drop = false);

/*
    Рисование линии на дисплее
*/
DllExport void draw_line(display_point start, display_point end, display_color color);

/*
    Рисование изображения на дисплее
*/
DllExport void draw_image(const char* path);

/*
    Состояния кнопок
*/
DllExport enum button_state
{
    FIRST_BUTTON_DOWN = 0x01,
    SECOND_BUTTON_DOWN = 0x02,
    THIRD_BUTTON_DOWN = 0x04,
    FOURTH_BUTTON_DOWN = 0x08
};

/*
    Получение состояния кнопок контроллера
*/
DllExport unsigned char get_buttons_state();

#endif // !LIB_SPACESHIELD