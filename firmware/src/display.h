#pragma once

#include <Arduino.h>
#include <Adafruit_ST7735.h>
#include <Fonts/Michroma6pt7b.h>
#include <Fonts/Michroma36pt7b.h>


#define LED_VALUE_MAX 20


struct colorContainer {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

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

        void setLedColor(uint8_t, uint8_t, uint8_t, bool pulse=false);
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

        void _setLedColor(uint8_t, uint8_t, uint8_t);
        void _ledPulse();

        uint8_t lcd_cs;
        uint8_t lcd_dc;
        uint8_t lcd_rst;

        bool led_pulse_enabled;
        uint8_t led_r_target;
        uint8_t led_g_target;
        uint8_t led_b_target;
        unsigned long last_led_pulse;
        float led_pulse_position;

        uint8_t led_r;
        uint8_t led_g;
        uint8_t led_b;

        SPIClass* spi;
};
