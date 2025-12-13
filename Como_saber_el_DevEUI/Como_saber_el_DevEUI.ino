#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  uint64_t chipid = ESP.getEfuseMac(); // MAC base del ESP32
  uint8_t devEui[8];

  devEui[0] = (chipid >> 56) & 0xFF;
  devEui[1] = (chipid >> 48) & 0xFF;
  devEui[2] = (chipid >> 40) & 0xFF;
  devEui[3] = (chipid >> 32) & 0xFF;
  devEui[4] = (chipid >> 24) & 0xFF;
  devEui[5] = (chipid >> 16) & 0xFF;
  devEui[6] = (chipid >> 8)  & 0xFF;
  devEui[7] = chipid & 0xFF;

  Serial.print("DevEUI: ");
  for (int i = 0; i < 8; i++) {
    if (devEui[i] < 0x10) Serial.print("0");
    Serial.print(devEui[i], HEX);
    if (i < 7) Serial.print(":");
  }
  Serial.println();
}

void loop() {
}
