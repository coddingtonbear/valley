#include "display.h"

#define LEDC_TIMER_BIT 13
#define LEDC_BASE_FREQ 5001

#define LEDC_R 0
#define LEDC_G 1
#define LEDC_B 2

Display::Display(
    SPIClass* spi_bus,
    uint8_t _lcd_cs, uint8_t _lcd_dc, uint8_t _lcd_rst,
    uint8_t _led_r, uint8_t _led_g, uint8_t _led_b
) {
    led_pulse_position = 0;
    led_pulse_enabled = false;

    spi = spi_bus;

    lcd_cs = _lcd_cs;
    lcd_dc = _lcd_dc;
    lcd_rst = _lcd_rst;
    led_r = _led_r;
    led_g = _led_g;
    led_b = _led_b;

    pinMode(led_r, OUTPUT);
    pinMode(led_g, OUTPUT);
    pinMode(led_b, OUTPUT);

    // Configure pins for PWM mode
    ledcSetup(LEDC_R, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcAttachPin(led_r, LEDC_R);
    ledcSetup(LEDC_G, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcAttachPin(led_g, LEDC_G);
    ledcSetup(LEDC_B, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
    ledcAttachPin(led_b, LEDC_B);

    pinMode(lcd_cs, OUTPUT);
    pinMode(lcd_rst, OUTPUT);
    pinMode(lcd_dc, OUTPUT);
}

Display::Display() {};

void Display::setup()
{
    setLedColor(100, 0, 0);

    lcd = new Adafruit_ST7735(spi, lcd_cs, lcd_dc, lcd_rst);

    lcd->initR(INITR_18BLACKTAB);
    lcd->setRotation(1);
    lcd->setTextWrap(true);

    setTextColor(255, 255, 255);
    setLedColor(255, 255, 255);
    fillScreen(0, 0, 0);
}

void Display::setText(String value) {
    lcd->setFont(&Michroma6pt7b);
    lcd->setCursor(0, 10);
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
    ledcWrite(LEDC_R, r ? (8191 / r) : 0 * 255);
    ledcWrite(LEDC_G, g ? (8191 / g) : 0 * 255);
    ledcWrite(LEDC_B, b ? (8191 / b) : 0 * 255);
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
