#include "main.h"

SPIClass hspi;
Display display;
RescueTime rescuetime = RescueTime(String(RESCUETIME_API_KEY));

bool connected = false;
unsigned long lastSample = 0;
unsigned long lastRefresh = 0;

float lastProductivityScoreDisplayed = -1;
float productivityScoreDisplayed = -1;
int productivityScore = 0;

void setup()
{
    Serial.begin(115200);

    hspi = SPIClass(HSPI);
    hspi.begin(14, 12, 13);

    display = Display(
        &hspi, LCD_CS, LCD_DC, LCD_RST,
        LED_R, LED_G, LED_B
    );
    display.setup();

    display.fillScreen(0, 0, 30);
    display.setLedColor(0, 0, 100, EFFECT_BREATHE);
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    Serial.print("Connecting to '" + String(WIFI_SSID) + "'...");
    long started = millis();
    while(WiFi.status() != WL_CONNECTED) {
        display.setText("Connecting to\n'" + String(WIFI_SSID) + "'...");
        display.loop();

        if (millis() > started + WIFI_FAILURE_RESTART) {
            display.fillScreen(255, 0, 0);
            display.setTextColor(255, 255, 255);
            display.setBigText("W" + String(WiFi.status()));
            delay(5000);
            ESP.restart();
        }
    }
    display.fillScreen(0, 0, 60);
    display.setText("Connected as \n'" + WiFi.localIP().toString() + "'");
    Serial.println();
    Serial.println("Connected as " + WiFi.localIP().toString());
    delay(3000);

    pinMode(SD_CS, OUTPUT);

    digitalWrite(LED_R, 0);
    digitalWrite(LED_G, 0);
    digitalWrite(LED_B, 0);
}


void loop()
{
    display.loop();

    if(digitalRead(NOT_USB_ENUMERATED)) {
        display.setLedColor(100, 0, 0, EFFECT_BREATHE);
        display.fillScreen(0, 0, 0);
        return;
    }

    if(lastSample == 0 || (millis() - lastSample > SAMPLE_INTERVAL)) {
        float productivityScoreResult = rescuetime.getProductivityScore();
        if(productivityScoreResult >= 0) {
            productivityScore = productivityScoreResult;
            lastSample = millis();
            Serial.print("Current Productivity: ");
            Serial.println(productivityScore);
        } else {
            display.fillScreen(255, 0, 0);
            display.setTextColor(255, 255, 255);
            display.setBigText("E" + String(abs(productivityScoreResult)));
            delay(5000);

            productivityScoreDisplayed = -1;
        }
    }

    if(productivityScore != productivityScoreDisplayed) {
        lastProductivityScoreDisplayed = productivityScoreDisplayed;
        productivityScoreDisplayed = productivityScore;

        int roundedProductivityScore = (int)round(productivityScore);

        colorContainer textColor = getTextColorForProductivityScore(
            roundedProductivityScore
        );
        colorContainer bgColor = getBackgroundColorForProductivityScore(
            roundedProductivityScore
        );

        display.fillScreen(bgColor.r, bgColor.g, bgColor.b);
        display.setTextColor(textColor.r, textColor.g, textColor.b);
        Serial.print("BGColor: #");
        Serial.print(bgColor.r, HEX);
        Serial.print(bgColor.g, HEX);
        Serial.println(bgColor.b, HEX);
        Serial.print("TxtColor: #");
        Serial.print(textColor.r, HEX);
        Serial.print(textColor.g, HEX);
        Serial.println(textColor.b, HEX);

        if(productivityScore < 100) {
            display.setBigText(String(roundedProductivityScore));
        } else {
            display.setBigText("OK");
        }

        if(lastProductivityScoreDisplayed >= 0) {
            if(lastProductivityScoreDisplayed > productivityScoreDisplayed) {
                display.setLedColor(5, 10, 0, EFFECT_BREATHE);
            } else {
                display.setLedColor(0, 10, 5, EFFECT_BREATHE);
            }
        } else {
            display.setLedColor(5, 5, 5, EFFECT_BREATHE);
        }

    }
}


colorContainer getTextColorForProductivityScore(int score) {
    colorContainer color;

    if (score >= 50) {
        color.r = 0;
        color.g = 0;
        color.b = 0;
    } else {
        color = display.calculateIntermediate(
            0, 0, 0,
            200, 0, 0,
            (float)score/50
        );
    }

    return color;
}

colorContainer getBackgroundColorForProductivityScore(int score) {
    colorContainer color;

    if (score >= 50) {
        color = display.calculateIntermediate(
            200, 0, 0,
            0, 200, 0,
            (float)(_max(score - 50, 0)) / 50
        );
    } else {
        color = display.calculateIntermediate(
            0, 0, 0,
            200, 0, 0,
            (float)score/50
        );
    }

    return color;
}
