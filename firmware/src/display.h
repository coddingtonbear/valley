#pragma once

#include <Arduino.h>
#include <LEDEffect.h>
#include <Adafruit_ST7735.h>
#include <Fonts/Michroma6pt7b.h>
#include <Fonts/Michroma36pt7b.h>

#define LEDC_TIMER_BIT 11
#define LEDC_BASE_FREQ 5000

#define LEDC_R 0
#define LEDC_G 1
#define LEDC_B 2

#define EFFECT_ON 0
#define EFFECT_OFF 1
#define EFFECT_BREATHE 2
#define EFFECT_BLINK 3

#define EFFECT_BREATHE_DURATION 5000
#define EFFECT_BLINK_DURATION 500


struct colorContainer {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

void setTimerBrightness(uint8_t timer_id, uint8_t brightness);

class Display
{
    public:
        Display(
            SPIClass*,
            uint8_t, uint8_t, uint8_t,
            uint8_t, uint8_t, uint8_t
        );
        Display();
        void setup();
        void loop();

        void setLedColor(uint8_t, uint8_t, uint8_t, uint8_t effect=EFFECT_ON);
        void fillScreen(uint8_t, uint8_t, uint8_t);
        void setTextColor(uint8_t, uint8_t, uint8_t);
        colorContainer calculateIntermediate(
            int, int, int,
            int, int, int,
            float
        );
        void setText(String);
        void setBigText(String);

    private:
        Adafruit_ST7735* lcd;

        void _setLedColor(uint8_t, uint8_t, uint8_t, uint8_t effect=EFFECT_ON);

        uint8_t lcd_cs;
        uint8_t lcd_dc;
        uint8_t lcd_rst;

        uint8_t led_r_target;
        uint8_t led_g_target;
        uint8_t led_b_target;

        uint8_t led_r;
        uint8_t led_g;
        uint8_t led_b;

        LEDEffect effect_r;
        LEDEffect effect_g;
        LEDEffect effect_b;

        SPIClass* spi;
};
