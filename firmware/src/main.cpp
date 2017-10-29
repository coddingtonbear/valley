#include "main.h"

SPIClass hspi;
Display display;
RescueTime rescuetime = RescueTime(String(RESCUETIME_API_KEY));

bool connected = false;
unsigned long lastSample = 0;
unsigned long lastRefresh = 0;

int productivityScoreDisplayed = -1;
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
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    Serial.println("Connecting to\n '" + String(WIFI_SSID) + "'");
    long started = millis();
    while(WiFi.status() != WL_CONNECTED) {
        display.setText("Connecting to\n'" + String(WIFI_SSID) + "'...");
        Serial.print('.');
        delay(500);

        if (millis() > started + WIFI_FAILURE_RESTART) {
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
    if(digitalRead(NOT_USB_ENUMERATED)) {
        display.setLedColor(100, 0, 0);
        display.fillScreen(0, 0, 0);
        return;
    }

    if(lastSample == 0 || (millis() - lastSample > SAMPLE_INTERVAL)) {
        int productivityScoreResult = rescuetime.getProductivityScore();
        if(productivityScoreResult >= 0) {
            productivityScore = productivityScoreResult;
            lastSample = millis();
        } else {
            display.fillScreen(255, 0, 0);
            display.setTextColor(255, 255, 255);
            display.setBigText("E" + String(abs(productivityScoreResult)));
            delay(5000);

            productivityScoreDisplayed = -1;
        }
    }

    if(productivityScore != productivityScoreDisplayed) {
        productivityScoreDisplayed = productivityScore;

        colorContainer textColor = getTextColorForProductivityScore(
            productivityScore
        );
        colorContainer bgColor = getBackgroundColorForProductivityScore(
            productivityScore
        );

        display.fillScreen(bgColor.r, bgColor.g, bgColor.b);
        display.setTextColor(textColor.r, textColor.g, textColor.b);

        Serial.print("Current Productivity: ");
        Serial.println(productivityScore);
        Serial.print("BGColor: #");
        Serial.print(bgColor.r, HEX);
        Serial.print(bgColor.g, HEX);
        Serial.println(bgColor.b, HEX);
        Serial.print("TxtColor: #");
        Serial.print(textColor.r, HEX);
        Serial.print(textColor.g, HEX);
        Serial.println(textColor.b, HEX);

        if(productivityScore < 100) {
            display.setBigText(String(productivityScore));
        } else {
            display.setBigText("OK");
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
