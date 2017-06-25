#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EventManager.h>
#include <functional-vlpp.h>

#define MAX_REGISTERED_CALLBACKS 5
#define HEARTBEAT_INTERVAL 2500
#define HEARTBEAT_TIMEOUT 5000

struct callback {
    unsigned long messageId;
    vl::Func<void(JsonObject&)> fn;
};

class Comm
{
    public:
        Comm(EventManager*, Stream*);

        //void sendHello(vl::Func<void(JsonObject&)>);

        void sendHello();
        void setup();
        void loop();
        unsigned long sendMessage(
            String cmd,
            JsonObject&
        );
        unsigned long sendMessageWithCallback(
            String cmd,
            JsonObject&,
            vl::Func<void(JsonObject&)>
        );
        unsigned long debugMessage(String message);
    private:
        EventManager* events;
        Stream* port;
        bool connected;
        unsigned long lastHeartbeat;
        unsigned long lastHeartbeatReply;

        uint8_t currentIndex;
        callback* callbackList[MAX_REGISTERED_CALLBACKS];
};
