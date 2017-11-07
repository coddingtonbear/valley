#include "display.h"


uint8_t current_r = 0;
uint8_t current_g = 0;
uint8_t current_b = 0;
uint8_t current_effect = -1;


void setTimerBrightness(uint8_t timer_id, int brightness) {
    ledcWrite(timer_id, _min(brightness, pow(2, LEDC_TIMER_BIT) - 1));
}


Display::Display(
    SPIClass* spi_bus,
    uint8_t _lcd_cs, uint8_t _lcd_dc, uint8_t _lcd_rst,
    uint8_t _led_r, uint8_t _led_g, uint8_t _led_b
) {
    spi = spi_bus;

    led_mutex = xSemaphoreCreateMutex();

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

    effect_r = LEDEffect([](int value){
        //Serial.print("R: ");
        //Serial.println(value);
        setTimerBrightness(LEDC_R, value);
    });
    effect_g = LEDEffect([](int value){
        //Serial.print("G: ");
        //Serial.println(value);
        setTimerBrightness(LEDC_G, value);
    });
    effect_b = LEDEffect([](int value){
        //Serial.print("B: ");
        //Serial.println(value);
        setTimerBrightness(LEDC_B, value);
    });

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

void Display::setLedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t effect)
{
    _setLedColor(r, g, b, effect);
}

void Display::_setLedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t effect)
{
    xSemaphoreTake(led_mutex, portMAX_DELAY);
    if(r != current_r || g != current_g || b != current_b || effect != current_effect) {
        Serial.print("Setting LED Color: #");
        Serial.print(r, HEX);
        Serial.print(g, HEX);
        Serial.println(b, HEX);

        effect_r.reset();
        effect_g.reset();
        effect_b.reset();

        effect_r.setMax((pow(2, LEDC_TIMER_BIT) / 255) * r);
        effect_g.setMax((pow(2, LEDC_TIMER_BIT) / 255) * g);
        effect_b.setMax((pow(2, LEDC_TIMER_BIT) / 255) * b);

        if(effect == EFFECT_ON) {
            effect_r.on();
            effect_g.on();
            effect_b.on();
        } else if (effect == EFFECT_OFF) {
            effect_r.off();
            effect_g.off();
            effect_b.off();
        } else if (effect == EFFECT_BREATHE) {
            effect_r.breath(EFFECT_BREATHE_DURATION);
            effect_g.breath(EFFECT_BREATHE_DURATION);
            effect_b.breath(EFFECT_BREATHE_DURATION);
        } else if (effect == EFFECT_BLINK) {
            effect_r.blink(EFFECT_BLINK_DURATION);
            effect_g.blink(EFFECT_BLINK_DURATION);
            effect_b.blink(EFFECT_BLINK_DURATION);
        }

        current_r = r;
        current_g = g;
        current_b = b;
        current_effect = effect;
    }
    xSemaphoreGive(led_mutex);
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
    xSemaphoreTake(led_mutex, portMAX_DELAY);
    effect_r.update();
    effect_g.update();
    effect_b.update();
    xSemaphoreGive(led_mutex);
}
