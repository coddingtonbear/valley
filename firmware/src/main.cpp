#include "main.h"


Display display = Display(
    LCD_CS, LCD_DC, LCD_RST,
    LED_R, LED_G, LED_B, BACKLIGHT
);


void setup()
{
    pinMode(LC_ENABLE, OUTPUT);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(LC_ENABLE, HIGH);

    display.setup();

    Serial.begin(9600);
}


void loop()
{
    Serial.println(millis());
    display.loop();
    delay(1000);
}
