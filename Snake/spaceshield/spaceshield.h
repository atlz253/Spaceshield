void init_shield();
void close_shield();

#define LED_GREEN {0, 5, 0}
#define LED_RED {5, 0, 0}

typedef struct led_color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} led_color;

led_color get_led_color();
void set_led_color(led_color color);


unsigned char get_resistor_value();


typedef struct display_point
{
    unsigned char x;
    unsigned char y;
    unsigned char color;
} display_point;

void clear_display();

void draw_pixel(display_point point);

enum button_state
{
    FIRST_BUTTON_DOWN = 0x01,
    SECOND_BUTTON_DOWN = 0x02,
    THIRD_BUTTON_DOWN = 0x04,
    FOURTH_BUTTON_DOWN = 0x08
};

unsigned char get_buttons_state();