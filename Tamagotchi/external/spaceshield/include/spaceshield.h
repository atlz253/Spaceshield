#define DllExport   __declspec( dllexport )

DllExport void init_shield();
DllExport void close_shield();

#define LED_GREEN {0, 5, 0}
#define LED_RED {5, 0, 0}

typedef DllExport struct led_color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} led_color;

DllExport led_color get_led_color();
DllExport void set_led_color(led_color color);


DllExport unsigned char get_resistor_value();


typedef DllExport struct display_point
{
    unsigned char x;
    unsigned char y;
} display_point;

DllExport enum display_color
{
    DISPLAY_COLOR_BLACK = 0x00,
    DISPLAY_COLOR_WHITE = 0x01
};

DllExport void display_fill(display_color color);

DllExport void draw_pixel(display_point point, display_color color);
DllExport void draw_poly_pixel(int count, unsigned char *pixels, bool allow_drop = false);
DllExport void draw_line(display_point start, display_point end, display_color color);
DllExport void draw_image(const char* path);

DllExport enum button_state
{
    FIRST_BUTTON_DOWN = 0x01,
    SECOND_BUTTON_DOWN = 0x02,
    THIRD_BUTTON_DOWN = 0x04,
    FOURTH_BUTTON_DOWN = 0x08
};

DllExport unsigned char get_buttons_state();