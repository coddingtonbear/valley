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

    Serial.setTimeout(250);
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
    display.setBacklight(false);
    connected = true;
}


void onHeartbeatTimeout(int eventCode, String* eventData) {
    connected = false;
    display.setBacklight(false);
    display.setLedColor(100, 50, 0);

    delete eventData;
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
        root["path"] = "$.rows[*][4]";

        //comm.debugMessage("Fetching data from rescuetime...");
        comm.sendMessageWithCallback(
            "get_json",
            root,
            [&display,&lastUpdateReceived](JsonObject& result) {
                //comm.debugMessage("Data received.");
                int lastProductivity = (int)(
                    result["matches"][result["matches"].size() - 1]
                );
                //comm.debugMessage(
                //    "Found productivity numbers: " + String(lastProductivity)
                //);
                lastUpdateReceived = millis();

                colorContainer displayColor = display.calculateIntermediate(
                    255, 0, 0,
                    0, 255, 0,
                    (float)(lastProductivity - 50) / 50
                );
                //Serial.println("R: " + String(displayColor.r));
                //Serial.println("G: " + String(displayColor.g));
                //Serial.println("B: " + String(displayColor.b));

                display.fillScreen(
                    displayColor.r,
                    displayColor.g,
                    displayColor.b
                );
                display.setBigText(
                    String((int)lastProductivity) + " %"
                );
                display.setBacklight(true);
            }
        );
    }
}
