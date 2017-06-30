#include "display.h"

Display::Display(
    uint8_t _lcd_cs, uint8_t _lcd_dc, uint8_t _lcd_rst,
    uint8_t _led_r, uint8_t _led_g, uint8_t _led_b,
    uint8_t _backlight
) {
    led_pulse_position = 0;
    led_pulse_enabled = false;

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
    lcd->setFont(&Michroma12pt7b);
    lcd->setCursor(10, 30);
    lcd->println(value);
}

void Display::setBigText(String value) {
    lcd->setFont(&Michroma36pt7b);
    lcd->setCursor(12, 90);
    lcd->println(value);
}

void Display::setLedColor(uint8_t r, uint8_t g, uint8_t b, bool pulse)
{
    if(pulse) {
        last_led_pulse = millis();
        //led_pulse_position = 0;
        led_pulse_enabled = pulse;

        led_r_target = r;
        led_g_target = g;
        led_b_target = b;
    } else {
        _setLedColor(r, g, b);
    }
}

void Display::_setLedColor(uint8_t r, uint8_t g, uint8_t b)
{
    analogWrite(led_r, (int)r * (float)LED_VALUE_MAX / (float)255);
    analogWrite(led_g, (int)g * (float)LED_VALUE_MAX / (float)255);
    analogWrite(led_b, (int)b * (float)LED_VALUE_MAX / (float)255);
}

void Display::_ledPulse() {
    if(led_pulse_enabled) {
        //Serial.println("millis(): " + String(millis()));
        //Serial.println("last_led_pulse: " + String(last_led_pulse));
        //Serial.println("LED_PULSE_PERIOD: " + String(LED_PULSE_PERIOD));
        //led_pulse_position += (float)(
        //    (float)(millis() - last_led_pulse) / (float)LED_PULSE_PERIOD * (2 * PI)
        //);
        led_pulse_position += 0.001;
        //Serial.println("Calculated pulse position: " + String(led_pulse_position));
        if (led_pulse_position > (2 * PI)) {
            led_pulse_position = 0;
            //led_pulse_position = fmod(led_pulse_position, (2 * PI));
            //Serial.println("Current Pulse Position Updated to :" + String(led_pulse_position));
        }
        float multiplier = sin(led_pulse_position) * 0.25 + 0.75;

        //Serial.println("Targets:");
        //Serial.println(
        //    " R: " + String((int)(led_r_target)) +
        //    " G: " + String((int)(led_g_target)) +
        //    " B: " + String((int)(led_b_target))
        //);
        //Serial.println("Multiplier: " + String(multiplier));
        //Serial.println("Values:");
        //Serial.println(
        //    " R: " + String((int)(multiplier * led_r_target)) +
        //    " G: " + String((int)(multiplier * led_g_target)) +
        //    " B: " + String((int)(multiplier * led_b_target))
        //);
        _setLedColor(
            (int)(multiplier * led_r_target),
            (int)(multiplier * led_g_target),
            (int)(multiplier * led_b_target)
        );
        last_led_pulse = millis();
    }
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
    lcd->fillScreen(lcd->Color565(b, g, r));
}

void Display::setTextColor(uint8_t r, uint8_t g, uint8_t b) {
    lcd->setTextColor(lcd->Color565(b, g, r));
}

void Display::loop()
{
    _ledPulse();
}
