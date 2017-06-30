#include "main.h"


EventManager events;
Display display = Display(
    LCD_CS, LCD_DC, LCD_RST,
    LED_R, LED_G, LED_B, BACKLIGHT
);
Comm comm = Comm(&events, &Serial);


bool connected = false;
unsigned long lastSample = 0;
unsigned long lastUpdateReceived = 0;


void setup()
{
    pinMode(LC_ENABLE, OUTPUT);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(LC_ENABLE, HIGH);

    display.setup();

    Serial.setTimeout(50);
    Serial.begin(115200);

    events.addListener(
        EventManager::kEventHeartbeatTimeout,
        onHeartbeatTimeout
    );
    events.addListener(
        EventManager::kEventConnected,
        onConnected
    );
}


void onConnected(int eventCode, String* eventData) {
    display.setLedColor(0, 255, 0);
    display.setBacklight(true);
    connected = true;

    //delete eventData;
}


void onHeartbeatTimeout(int eventCode, String* eventData) {
    connected = false;
    display.setBacklight(false);
    display.setLedColor(100, 50, 0);

    //delete eventData;
}


void loop()
{
    display.loop();
    comm.loop();
    events.processAllEvents();

    unsigned long minValue = 0;
    if (millis() > REFRESH_INTERVAL) {
        minValue = millis() - REFRESH_INTERVAL;
    }
    if(
        connected &&
        (lastSample == 0 | lastSample < minValue)
    ) {
        lastSample = millis();

        StaticJsonBuffer<200> jsonBuffer;

        JsonObject& root = jsonBuffer.createObject();
        root["url"] = (
            "https://www.rescuetime.com/anapi/data?key="
            RESCUETIME_API_KEY
            "&perspective=interval"
            "&restrict_kind=efficiency"
            "&interval=hour"
        );
        root["path"] = "$.rows[*]";

        //comm.debugMessage("Fetching data from rescuetime...");
        comm.sendMessageWithCallback(
            "get_json",
            root,
            [&display,&lastUpdateReceived](JsonObject& result) {
                comm.debugMessage("Data received.");
                int rowCount = result["matches"].size();
                float productivityValue = 0;
                int totalSeconds = 0;
                for(int i = rowCount - 1; i >= 0; i--) {
                    int transferrableSeconds = min(
                        LOOKBACK_SECONDS - totalSeconds,
                        result["matches"][i][1].as<int>()
                    );
                    productivityValue += (
                        result["matches"][i][4].as<float>() *
                        transferrableSeconds
                    );
                    totalSeconds += transferrableSeconds;
                    //comm.debugMessage(
                    //    "Adding " + String(transferrableSeconds) + " at " +
                    //    String(result["matches"][i][4].as<float>()) + "%"
                    //);
                }
                int displayedProductivity = (int)(
                    productivityValue / totalSeconds
                );
                lastUpdateReceived = millis();

                colorContainer displayColor = display.calculateIntermediate(
                    200, 0, 0,
                    0, 200, 0,
                    (float)(max(displayedProductivity - 50, 0)) / 50
                );
                comm.debugMessage(
                    "Setting screen to "
                    "R: " + String(displayColor.r)
                    + " G: " + String(displayColor.g)
                    + " B: " + String(displayColor.b)
                );
                display.setTextColor(0, 0, 0);
                display.fillScreen(
                    displayColor.r,
                    displayColor.g,
                    displayColor.b
                );
                if(displayedProductivity < 100) {
                    display.setBigText(String(displayedProductivity));
                } else {
                    display.setBigText("OK");
                }
                display.setBacklight(true);
            }
        );
    }
}
