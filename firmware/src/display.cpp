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
    digitalWrite(backlight, LOW);
    setLedColor(100, 0, 0);

    lcd = new Adafruit_ST7735(lcd_cs, lcd_dc, lcd_rst);

    lcd->initR(INITR_18BLACKTAB);
    lcd->setRotation(1);
    lcd->setTextWrap(true);

    setTextColor(255, 255, 255);
    setLedColor(100, 100, 100);
    fillScreen(0, 0, 0);
}

void Display::setText(String value) {
    lcd->setFont(&FreeSans12pt7b);
    lcd->setCursor(10, 30);
    lcd->println(value);
}

void Display::setBigText(String value) {
    lcd->setFont(&FreeSansBold24pt7b);
    lcd->setCursor(10, 50);
    lcd->println(value);
}

void Display::setLedColor(uint8_t r, uint8_t g, uint8_t b)
{
    analogWrite(led_r, (int)r * (float)20 / (float)255);
    analogWrite(led_g, (int)g * (float)20 / (float)255);
    analogWrite(led_b, (int)b * (float)20 / (float)255);
}

colorContainer Display::calculateIntermediate(
    int ar, int ag, int ab,
    int br, int bg, int bb,
    float value
) {
    int dr = br - ar;
    int dg = bg - ag;
    int db = bb - ab;

    colorContainer result = colorContainer();
    //Serial.println("DR: " + String(dr));
    //Serial.println("DG: " + String(dg));
    //Serial.println("DB: " + String(db));
    //Serial.println("VAL: " + String(value));
    result.r = (uint8_t)((dr * value) + ar);
    result.g = (uint8_t)((dg * value) + ag);
    result.b = (uint8_t)((db * value) + ab);

    return result;
}

void Display::setBacklight(bool enabled)
{
    digitalWrite(backlight, enabled ? HIGH : LOW);
}

void Display::fillScreen(uint8_t r, uint8_t g, uint8_t b)
{
    lcd->fillScreen(lcd->Color565(r, g, b));
}

void Display::setTextColor(uint8_t r, uint8_t g, uint8_t b) {
    lcd->setTextColor(lcd->Color565(r, g, b));
}

void Display::loop()
{
}
