#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WiFiManager{
    private:
        const char *ssid;
        const char *password;
        IPAddress ip;
    public:
        WiFiManager(const char *ssid, const char *password);
        void beginAP();
        IPAddress getIP();
        uint8_t getClientNum();
};