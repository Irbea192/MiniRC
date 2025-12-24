#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "WiFiManager.h"
#include "Buzzer.h"

const int BattVoltPin = A0;
const int LedPin = D8;
const int BuzzerPin = D9;

const char* ssid = "esp32";
const char* password = "12345678";

WiFiManager wifiManager(ssid, password);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

float readBatVolt(uint8_t pin);
void setupServerRoutes(AsyncWebServer &server);
// void handleRoot();
// void handleNotFound();

void notifyClients() {
    StaticJsonDocument<128> doc;
    doc["vbatt"] = readBatVolt(BattVoltPin);
    String msg;
    serializeJson(doc, msg);
    ws.textAll(msg);  // 全クライアントに送信
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            String msg = String((char*)data);
            // 例: {"volume":50}
            StaticJsonDocument<128> doc;
            DeserializationError err = deserializeJson(doc, msg);
            if (!err) {
                if (doc.containsKey("volume")) {
                    int vol = doc["volume"];
                    Serial.printf("Volume set: %d\n", vol);
                    // 必要ならここでボリューム値を反映
                }
                // 他の操作もここで処理
            }
        }
    }
}

void setup() {
	Serial.begin(115200);
  delay(2000);

  Serial.printf("Setup start !");
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, HIGH);
  pinMode(BattVoltPin, INPUT);
  wifiManager.beginAP();

  server.addHandler(&ws);
  ws.onEvent(onWebSocketEvent);

  setupServerRoutes(server);
  server.begin();

  playDroneBootSound(BuzzerPin);

  Serial.printf("Set up done !\n");
}

void loop() {
  // readBatVolt(BattVoltPin);
  Serial.printf("Connected clients: %d, IP Address: %s\n", wifiManager.getClientNum(), wifiManager.getIP().toString().c_str());
  notifyClients();
  delay(1000);
}

float readBatVolt(uint8_t pin){
  int raw = analogRead(pin); // ADC_PINは使用ピン番号
  float vref = 3.49;
  float vin = (raw / 4095.0) * vref;
  float vbatt = vin * 2.0;
  Serial.printf("ADC: %d, Vin: %.2fV, Vbatt: %.2fV\n", raw, vin, vbatt);
  return vbatt;
}

void setupServerRoutes(AsyncWebServer &server) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
            <meta charset='utf-8'>
            <title>ESP32-S3 WebSocket Demo</title>
            </head>
            <body>
            <h1>ESP32-S3 WebSocket Demo</h1>
            <p>バッテリ電圧: <span id='vbatt'>--</span> V</p>
            <p>ボリューム: <input type='range' min='0' max='100' value='50' id='volume'> <span id='val'>50</span></p>
            <script>
                let ws = new WebSocket('ws://' + location.host + '/ws');
                ws.onmessage = function(event) {
                    let data = JSON.parse(event.data);
                    if(data.vbatt !== undefined) {
                        document.getElementById('vbatt').innerText = data.vbatt.toFixed(2);
                    }
                };
                document.getElementById('volume').oninput = function() {
                    let val = this.value;
                    document.getElementById('val').innerText = val;
                    ws.send(JSON.stringify({volume: parseInt(val)}));
                };
            </script>
            </body>
            </html>
        )rawliteral";
        request->send(200, "text/html", html);
    });
}