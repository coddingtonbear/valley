#pragma once

#include <Arduino.h>
#include <Adafruit_ST7735.h>
#include <Fonts/helvetica12.h>
#include <Fonts/helvetica48.h>

struct colorContainer {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

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

        void setLedColor(uint8_t, uint8_t, uint8_t);
        void fillScreen(uint8_t, uint8_t, uint8_t);
        void setTextColor(uint8_t, uint8_t, uint8_t);
        colorContainer calculateIntermediate(
            int, int, int,
            int, int, int,
            float
        );
        void setText(String);
        void setBigText(String);

        void setBacklight(bool enabled=true);
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
