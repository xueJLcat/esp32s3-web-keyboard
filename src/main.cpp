#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "keyboard.h"
#include "web_server.h";

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println("\nStarting USB HID Keyboard...");

  if (!initKeyboardSystem()) {
    Serial.println("Keyboard system init failed!");
  }

  // AP 模式
  WiFi.mode(WIFI_AP);
  bool apOk = WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress ip = WiFi.softAPIP();
  Serial.printf("AP start: %s, IP: %s\n", apOk ? "OK" : "FAIL", ip.toString().c_str());

  setupHttpServer();
  Serial.println("HTTP server started. Open http://192.168.4.1 from phone.");
}

void loop() {
  delay(10);
}
