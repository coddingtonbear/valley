#include "comm.h"

Comm::Comm(EventManager* evts, Stream* serial_port):
    events(evts),
    port(serial_port),
    lastHeartbeat(0),
    lastHeartbeatReply(0),
    connected(false)
{
    for(uint8_t i = 0; i < MAX_REGISTERED_CALLBACKS; i++) {
        callbackList[i] = nullptr;
    }
}

void Comm::setup()
{
}

void Comm::loop()
{
    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& params = jsonBuffer.createObject();
    params["message"] = millis();

    if(
        millis() > HEARTBEAT_INTERVAL &&
        millis() - lastHeartbeat > HEARTBEAT_INTERVAL
    ) {
        lastHeartbeat = millis();
        sendMessageWithCallback(
            "heartbeat",
            params,
            [this](JsonObject& result) {
                if(!this->connected) {
                    this->connected = true;
                    this->events->queueEvent(
                        EventManager::kEventConnected,
                        new String()
                    );
                }
                this->lastHeartbeatReply = millis();
            }
        );
    }
    if(
        this->connected == true &&
        millis() > HEARTBEAT_TIMEOUT * 2 &&
        millis() - lastHeartbeatReply > HEARTBEAT_TIMEOUT
    ) {
        this->connected = false;
        events->queueEvent(
            EventManager::kEventHeartbeatTimeout,
            new String()
        );
    }

    if (port->available()) {
        String output = port->readStringUntil('\n');
        DynamicJsonBuffer jsonIncoming;
        JsonObject& root = jsonBuffer.parse(output);

        if(root.success()) {
            //debugMessage("Got result for message: " + root["msg_id"].as<String>());
            //String debugOutput;
            //root.printTo(debugOutput);
            //debugMessage("Decoded output: " + debugOutput);
            unsigned long msgId = root["msg_id"];
            for(uint8_t i = 0; i < MAX_REGISTERED_CALLBACKS; i++) {
                callback* thisCallbackPtr = callbackList[i];
                if(thisCallbackPtr != nullptr)
                {
                    callback thisCallback = *thisCallbackPtr;
                    if(thisCallback.messageId == msgId) {
                        thisCallback.fn(root["result"]);
                        //debugMessage("Found matching callback!");
                        break;
                    } else {
                        /*debugMessage(
                            String(thisCallback.messageId) +
                            " != " +
                            String(msgId)
                        );*/
                    }
                }
            }
        } else {
            /*debugMessage(
                "Unable to decode output; received " +
                String(output.length()) +
                " bytes"
            );*/
        }
    }
    for(uint8_t i = 0; i < MAX_REGISTERED_CALLBACKS; i++) {
        callback* thisCallbackPtr = callbackList[i];
        if(thisCallbackPtr != nullptr)
        {
            callback thisCallback = *thisCallbackPtr;
            //debugMessage(
            //    "Callback found for message ID: " + 
            //    String(thisCallback.messageId)
            //);
        }
    }
}

unsigned long Comm::sendMessage(String cmd, JsonObject& params)
{
    StaticJsonBuffer<200> jsonBuffer;
    unsigned long message_id = millis();

    JsonObject& root = jsonBuffer.createObject();
    root["msg_id"] = message_id;
    root["cmd"] = cmd;
    root["params"] = params;

    port->write('\n');
    root.printTo(*port);
    port->write('\n');

    return message_id;
}



unsigned long Comm::sendMessageWithCallback(
    String cmd,
    JsonObject& params,
    vl::Func<void(JsonObject&)> fn
)
{
    unsigned long messageId = sendMessage(cmd, params);

    /* If we have a callback registered there; free it */
    if(callbackList[currentIndex] != nullptr) {
        delete callbackList[currentIndex];
    }
    /* Then create a new one */
    callback* thisCallback = new callback();
    thisCallback->messageId = messageId;
    thisCallback->fn = fn;
    callbackList[currentIndex] = thisCallback;
    currentIndex++;
    if (currentIndex == MAX_REGISTERED_CALLBACKS)  {
        currentIndex = 0;
    }
    //debugMessage("New callback registered for " + String(messageId));
}

unsigned long Comm::debugMessage(String message) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["message"] = message;

    sendMessage("debug", root);
}
