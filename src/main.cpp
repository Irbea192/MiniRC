#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "WiFiManager.h"
#include "AsyncWeb.h"
#include "Buzzer.h"

const uint8_t BattVoltPin = A0;
const uint8_t LedPin = D8;
const uint8_t BuzzerPin = D9;

const char* ssid = "esp32";
const char* password = "12345678";

WiFiManager wifiManager(ssid, password);
AsyncWeb asyncWeb;

typedef struct {
    int8_t joyx = 0;
    int8_t joyy = 0;
    char joydir[3] = "";
} JoyStickData;
JoyStickData joyData;

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            String msg = String((char*)data);
            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, msg);
            if (!err) {
                if (doc["joydata"].is<JsonObject>()) {
                    JsonObject joyObj = doc["joydata"].as<JsonObject>();
                    joyData.joyx = joyObj["joyx"] | 0;
                    joyData.joyy = joyObj["joyy"] | 0;
                    const char* dir = joyObj["joydir"] | "";
                    strncpy(joyData.joydir, dir, sizeof(joyData.joydir) - 1);
                    joyData.joydir[sizeof(joyData.joydir) - 1] = '\0';
                    Serial.printf("X: %d, Y: %d, Dir: %s\n", joyData.joyx, joyData.joyy, joyData.joydir);  
                }
            }
        }
    }
}

float readBatVolt(uint8_t pin){
    uint16_t raw = analogRead(pin); // ADC_PINは使用ピン番号
    float vref = 3.49;
    float vin = (raw / 4095.0) * vref;
    float vbatt = vin * 2.0;
    // Serial.printf("ADC: %d, Vin: %.2fV, Vbatt: %.2fV\n", raw, vin, vbatt);
    return vbatt;
}

void setup() {
	Serial.begin(115200);
    delay(2000);

    Serial.printf("Setup start !");
    pinMode(LedPin, OUTPUT);
    digitalWrite(LedPin, HIGH);
    pinMode(BattVoltPin, INPUT);

    wifiManager.beginAP(); // WiFiアクセスポイントの開始
    asyncWeb.begin(onWebSocketEvent); // WebサーバーとWebSocketの初期化

    playDroneBootSound(BuzzerPin); // 起動音の再生

    Serial.printf("Set up done !\n");
}

void loop() {
    // Serial.printf("Connected clients: %d, IP Address: %s\n", wifiManager.getClientNum(), wifiManager.getIP().toString().c_str());
    asyncWeb.notifyClients(readBatVolt(BattVoltPin));
    delay(1000);
}