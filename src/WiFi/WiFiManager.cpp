#include <WiFiManager.h>

WiFiManager::WiFiManager(const char *ssid, const char *password) : ssid(ssid), password(password) {}

void WiFiManager::beginAP(){
    WiFi.softAP(ssid, password);
    ip = WiFi.softAPIP();
    Serial.printf("AP IP address: %s\n", ip.toString().c_str());
}

IPAddress WiFiManager::getIP(){
    return ip;
}

uint8_t WiFiManager::getClientNum(){
    return WiFi.softAPgetStationNum();
}