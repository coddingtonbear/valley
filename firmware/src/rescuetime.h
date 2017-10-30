#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define LOOKBACK_SECONDS 3600

class RescueTime
{
public:
    RescueTime(String);

    float getProductivityScore();

private:
    String api_key;
};
