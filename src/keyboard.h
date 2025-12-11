#pragma once
#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "config.h"

enum class KeyEventType : uint8_t {
    TEXT,       // 普通文本（字符串）
    SPECIAL,    // 特殊键（如 ENTER、BACKSPACE、F1 等，包含 Ctrl/Alt/Shift/Win）
    COMBO       // 组合键（如 LCTRL+C、LGUI+D 等）
};

struct KeyEvent {
    KeyEventType type;
    String       payload;  // TEXT: 完整字符串；SPECIAL: 单个键名；COMBO: "LCTRL+C"
};

extern USBHIDKeyboard HidKeyboard;
extern QueueHandle_t  keyEventQueue;

bool initKeyboardSystem();
bool enqueueText(const String &text);
bool enqueueSpecialKeyName(const String &keyName);
bool enqueueCombo(const String &combo);
void keyboardTask(void *pvParameters);
