#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SparkFun_TB6612.h>
#include "esp_camera.h"

#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"

#include "WiFiManager.h"
#include "AsyncWeb.h"
#include "Buzzer.h"

const int offsetA = -1;
const int offsetB = -1;

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
    while(!Serial);
    delay(8000);

    Serial.printf("Setup start !\n");
    
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG; // for streaming
    //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    if(config.pixel_format == PIXFORMAT_JPEG){
        if(psramFound()){
            Serial.println("PSRAM is enabled!");
            config.jpeg_quality = 10;
            config.fb_count = 2;
            config.grab_mode = CAMERA_GRAB_LATEST;
        } else {
            Serial.println("PSRAM is NOT enabled!");
            // Limit the frame size when PSRAM is not available
            config.frame_size = FRAMESIZE_SVGA;
            config.fb_location = CAMERA_FB_IN_DRAM;
        }
    } else {
        // Best option for face detection/recognition
        config.frame_size = FRAMESIZE_240X240;
        #if CONFIG_IDF_TARGET_ESP32S3
            config.fb_count = 2;
        #endif
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

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