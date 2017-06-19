#include "display.h"

Display::Display(
    uint8_t _lcd_cs, uint8_t _lcd_dc, uint8_t _lcd_rst,
    uint8_t _led_r, uint8_t _led_g, uint8_t _led_b,
    uint8_t _backlight
) {
    lcd_cs = _lcd_cs;
    lcd_dc = _lcd_dc;
    lcd_rst = _lcd_rst;
    led_r = _led_r;
    led_g = _led_g;
    led_b = _led_b;
    backlight = _backlight;

    pinMode(led_r, OUTPUT);
    pinMode(led_g, OUTPUT);
    pinMode(led_b, OUTPUT);

    pinMode(lcd_cs, OUTPUT);
    pinMode(lcd_rst, OUTPUT);
    pinMode(lcd_dc, OUTPUT);
    pinMode(backlight, OUTPUT);
}

void Display::setup()
{
    analogWrite(led_r, 10);
    analogWrite(led_g, 10);
    analogWrite(led_b, 10);

    digitalWrite(backlight, HIGH);
    
    lcd = new Adafruit_ST7735(lcd_cs, lcd_dc, lcd_rst);

    lcd->initR(INITR_18BLACKTAB);
    lcd->setRotation(1);
    lcd->setFont(&FreeSans24pt7b);
    lcd->setTextWrap(true);
}

void Display::loop()
{
    int randomR = random(0, 255);
    int randomG = random(0, 255);
    int randomB = random(0, 255);

    analogWrite(led_r, (int)(randomR * (float)20 / (float)255));
    analogWrite(led_g, (int)(randomG * (float)20 / (float)255));
    analogWrite(led_b, (int)(randomB * (float)20 / (float)255));

    lcd->fillScreen(lcd->Color565(randomR, randomG, randomB));

    lcd->setCursor(0, 40);
    lcd->setTextColor(ST7735_WHITE);
    lcd->println(millis());
}
