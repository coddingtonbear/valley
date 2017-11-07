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

WiFiMulti wifi;


void setup()
{
    Serial.begin(115200);

    hspi = SPIClass(HSPI);
    hspi.begin(14, 12, 13);

    pinMode(SD_CS, OUTPUT);

    digitalWrite(LED_R, 0);
    digitalWrite(LED_G, 0);
    digitalWrite(LED_B, 0);

    display = Display(
        &hspi, LCD_CS, LCD_DC, LCD_RST,
        LED_R, LED_G, LED_B
    );
    display.setup();
    display.fillScreen(0, 0, 30);
    display.setLedColor(0, 0, 100, EFFECT_BREATHE);

    xTaskCreatePinnedToCore(
        backgroundLoop,
        "backgroundLoop",
        4092,
        NULL,
        1,
        NULL,
        0
    );

    for (
        uint8_t i = 0;
        i < (sizeof(wifiPasscodes)/sizeof(wifiPasscode));
        i++
    ) {
        wifi.addAP(wifiPasscodes[i].ssid, wifiPasscodes[i].pwd);
    }

    Serial.print("Connecting to wifi...");
    long started = millis();
    display.setText("Connecting to wifi...");
    while(wifi.run() != WL_CONNECTED) {
        if (millis() > started + WIFI_FAILURE_RESTART) {
            display.fillScreen(255, 0, 0);
            display.setTextColor(255, 255, 255);
            display.setBigText("W" + String(wifi.run()));
            delay(5000);
            ESP.restart();
        }
    }
    display.fillScreen(0, 0, 0);
    display.setText("Connected as \n'" + WiFi.localIP().toString() + "'");
    Serial.println();
    Serial.println("Connected as " + WiFi.localIP().toString());
    delay(3000);
}

void backgroundLoop(void* pvParameters) {
    while(true) {
        display.loop();
        delay(10);
    }
}

void loop()
{
    if(digitalRead(NOT_USB_ENUMERATED)) {
        display.setLedColor(0, 0, 10, EFFECT_BREATHE);
        display.fillScreen(0, 0, 0);
        return;
    }

    if(lastSample == 0 || (millis() - lastSample > SAMPLE_INTERVAL)) {
        float productivityScoreResult = rescuetime.getProductivityScore();
        Serial.print("Current Productivity: ");
        Serial.println(productivityScore);
        if(productivityScoreResult >= 0) {
            productivityScore = productivityScoreResult;
            lastSample = millis();
        } else if(productivityScoreResult != -3) {
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
                display.setLedColor(20, 10, 0, EFFECT_BREATHE);
            } else if (lastProductivityScoreDisplayed == productivityScoreDisplayed) {
                display.setLedColor(10, 10, 10, EFFECT_BREATHE);
            } else {
                display.setLedColor(0, 20, 10, EFFECT_BREATHE);
            }
        } else {
            display.setLedColor(10, 10, 10, EFFECT_BREATHE);
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
            200, 0, 0,
            0, 0, 0,
            (float)(_max(score - 25, 0)) / 25
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
