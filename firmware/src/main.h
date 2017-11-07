#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 25

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Esp.h>

#include "rescuetime.h"
#include "configuration.h"
#include "display.h"
#include "rescuetime.h"

#define LED_R 5
#define LED_G 18
#define LED_B 19

#define LCD_CS 33
#define LCD_RST 25
#define LCD_DC 26

#define SD_CS 4

#define NOT_USB_ENUMERATED 21

#define SAMPLE_INTERVAL 60000

#define WIFI_FAILURE_RESTART 30000

void setup();
void loop();

colorContainer getTextColorForProductivityScore(int);
colorContainer getBackgroundColorForProductivityScore(int);
