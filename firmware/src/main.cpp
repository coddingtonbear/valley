#include "main.h"


Adafruit_ST7735 lcd = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST);


void setup() {
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);

    pinMode(LC_ENABLE, OUTPUT);
    pinMode(SD_CS, OUTPUT);
    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_RST, OUTPUT);
    pinMode(LCD_DC, OUTPUT);
    pinMode(BACKLIGHT, OUTPUT);

    analogWrite(LED_R, 10);
    analogWrite(LED_G, 10);
    analogWrite(LED_B, 10);

    digitalWrite(BACKLIGHT, HIGH);

    digitalWrite(LC_ENABLE, HIGH);

    lcd.initR(INITR_GREENTAB);
    lcd.fillScreen(ST7735_BLUE);
    lcd.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);

    Serial.begin(9600);
}


void loop() {
    lcd.fillScreen(ST7735_BLUE);
    lcd.fillScreen(ST7735_WHITE);

    analogWrite(LED_R, random(0, 20));
    analogWrite(LED_G, random(0, 20));
    analogWrite(LED_B, random(0, 20));
    delay(1000);
}
