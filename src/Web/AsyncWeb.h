#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class AsyncWeb{
    private:
        void setupServerRoutes(AsyncWebServer &server);
    public:
        const uint16_t port = 80;
        const char* wsPath = "/ws";

        AsyncWebServer server;
        AsyncWebSocket ws;

        AsyncWeb();
        void begin(AwsEventHandler handler);
        void notifyClients(float vbatt);
};
