#pragma once
#include <Arduino.h>

// WiFi AP
constexpr const char *AP_SSID     = "XiaoKeyboard";
constexpr const char *AP_PASSWORD = "12345678";

// HTTP
constexpr uint16_t HTTP_PORT = 80;

// 文本 & 队列
constexpr size_t   MAX_TEXT_QUEUE_LEN = 10;
constexpr size_t   MAX_TEXT_LENGTH    = 512;
constexpr uint32_t KEYSTROKE_DELAY_MS = 5;
constexpr uint32_t MOD_TAP_DELAY_MS   = 30;   // 延迟 20~50 ms 以匹配常见 HID 点按节奏
