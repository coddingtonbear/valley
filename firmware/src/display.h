#include <Adafruit_ST7735.h>
#include <Fonts/FreeSans24pt7b.h>


class Display
{
    public:
        Display(
            uint8_t, uint8_t, uint8_t,
            uint8_t, uint8_t, uint8_t,
            uint8_t
        );
        void setup();
        void loop();
    private:
        Adafruit_ST7735* lcd;

        uint8_t lcd_cs;
        uint8_t lcd_dc;
        uint8_t lcd_rst;

        uint8_t led_r;
        uint8_t led_g;
        uint8_t led_b;
        uint8_t backlight;
};
