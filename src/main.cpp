#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SparkFun_TB6612.h>

#include "WiFiManager.h"
#include "AsyncWeb.h"
#include "Buzzer.h"

const int offsetA = 1;
const int offsetB = 1;

const uint8_t BattVoltPin = A0;
const uint8_t PWMA = D1;
const uint8_t AIN1 = D2;
const uint8_t AIN2 = D3;
const uint8_t BIN1 = D4;
const uint8_t BIN2 = D5;
const uint8_t PWMB = D6;
const uint8_t ServoPin = D7;
const uint8_t LedPin = D8;
const uint8_t BuzzerPin = D9;
const uint8_t STBY = D10;

const float vref = 3.49;
int16_t angle = 90;
int16_t speed = 0;

const char* ssid = "esp32";
const char* password = "12345678";

WiFiManager wifiManager(ssid, password);
AsyncWeb asyncWeb;
Servo myServo;
Motor motorA(AIN1, AIN2, PWMA, offsetA, STBY); // AIN1, AIN2, PWMA
Motor motorB(BIN1, BIN2, PWMB, offsetB, STBY); // BIN1, BIN2, PWMB

typedef struct {
    int8_t joyx = 0;
    int8_t joyy = 0;
    char joydir[3] = "";
} JoyStickData;
JoyStickData joyData;

void servoWriteAngle(){
    // if(joyData.joyx == angle) return;

    if(joyData.joyx <= -40){
        // マッピング
        angle = map(joyData.joyx, -100, -40, 0, 89);
    } else if(joyData.joyx >= 40){
        // マッピング
        angle = map(joyData.joyx, 40, 100, 91, 180);
    } else{
        angle = 90; // 中立位置
    }
    myServo.write(angle);
}

void motorControl(){
    if(joyData.joyy >= 40){
        speed = map(joyData.joyy, 40, 100, 0, 255);
        forward(motorA, motorB, speed);
    } else if(joyData.joyy <= -40){
        speed = map(joyData.joyy, -40, -100, 0, -255);
        forward(motorA, motorB, speed);
    } else{
        speed = 0;
        brake(motorA, motorB);
    }
    Serial.printf("Speed: %d\n", speed);
}

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
                    servoWriteAngle();
                    motorControl();
                }
            }
        }
    }
}

float readBatVolt(uint8_t pin){
    uint16_t raw = analogRead(pin); // ADC_PINは使用ピン番号
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

    playDroneBootSound(BuzzerPin); // 起動音の再生

    myServo.setPeriodHertz(50); 
    myServo.attach(ServoPin);
    myServo.write(angle); // サーボを中立位置に設定

    wifiManager.beginAP(); // WiFiアクセスポイントの開始
    asyncWeb.begin(onWebSocketEvent); // WebサーバーとWebSocketの初期化

    Serial.printf("Set up done !\n");
}

void loop() {
    // Serial.printf("Connected clients: %d, IP Address: %s\n", wifiManager.getClientNum(), wifiManager.getIP().toString().c_str());
    asyncWeb.notifyClients(readBatVolt(BattVoltPin));
    delay(1000);
}