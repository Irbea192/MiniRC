#include <Arduino.h>
#include <ESP32Servo.h>

const int BattVoltPin = A0;
const int LedPin = D8;
const int BuzzerPin = D9;

void playDroneBootSound(int pin);
float readBatVolt(uint8_t pin);

void setup() {
	Serial.begin(115200);
  delay(2000);
  Serial.printf("Set up start !");
  pinMode(LedPin, OUTPUT);
  digitalWrite(LedPin, HIGH);
  pinMode(BattVoltPin, INPUT);

  playDroneBootSound(BuzzerPin);

  Serial.printf("Set up done !\n");
}

void loop() {
  readBatVolt(BattVoltPin);
  delay(1000);
}

void playDroneBootSound(int pin) {
  // 上昇音階
  int up[] = { 523, 587, 659, 784 }; // ドレミソ
  // 下降音階
  int down[] = { 784, 659, 587, 523 }; // ソミレド
  int duration = 100;

  // 上昇
  for (int i = 0; i < 4; i++) {
    tone(pin, up[i], duration);
    delay(duration * 1.2);
    noTone(pin);
  }
  // 下降
  for (int i = 0; i < 4; i++) {
    tone(pin, down[i], duration);
    delay(duration * 1.2);
    noTone(pin);
  }
}

float readBatVolt(uint8_t pin){
  int raw = analogRead(pin); // ADC_PINは使用ピン番号
  float vref = 3.3;
  float vin = (raw / 4095.0) * vref;
  float vbatt = vin * 2.0;
  Serial.printf("ADC: %d, Vin: %.2fV, Vbatt: %.2fV\n", raw, vin, vbatt);
  return vbatt;
}


