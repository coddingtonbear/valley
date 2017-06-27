#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 25

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EventManager.h>
#include <SPI.h>
#include "comm.h"
#include "configuration.h"
#include "display.h"

#define LED_R 12
#define LED_G 13
#define LED_B 14

#define LCD_CS 3
#define LCD_RST 0
#define LCD_DC 2

#define BACKLIGHT 10

#define LC_ENABLE 4

#define SD_CS 11

//#define REFRESH_INTERVAL 30000
#define REFRESH_INTERVAL 200000
#define LOOKBACK_SECONDS 3600

void setup();
void loop();

void onConnected(int eventCode, String* eventData);
void onHeartbeatTimeout(int eventCode, String* eventData);
