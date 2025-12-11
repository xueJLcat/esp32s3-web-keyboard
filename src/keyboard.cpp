#include "keyboard.h"

USBHIDKeyboard HidKeyboard;
QueueHandle_t  keyEventQueue      = nullptr;
TaskHandle_t   keyboardTaskHandle = nullptr;

struct SpecialKeyMap {
    const char *name;
    uint8_t     code;
    bool        isModifier;   // 是否是 Ctrl/Shift/Alt/Win 这种修饰键
};

// 可根据需要继续加
static const SpecialKeyMap SPECIAL_KEYS[] = {
    {"ENTER",     KEY_RETURN,     false},
    {"RETURN",    KEY_RETURN,     false},
    {"BACKSPACE", KEY_BACKSPACE,  false},
    {"TAB",       KEY_TAB,        false},
    {"ESC",       KEY_ESC,        false},
    {"ESCAPE",    KEY_ESC,        false},
    {"SPACE",     ' ',            false},   // 空格直接用 ASCII

    {"UP",        KEY_UP_ARROW,   false},
    {"DOWN",      KEY_DOWN_ARROW, false},
    {"LEFT",      KEY_LEFT_ARROW, false},
    {"RIGHT",     KEY_RIGHT_ARROW,false},

    {"DELETE",    KEY_DELETE,     false},
    {"DEL",       KEY_DELETE,     false},
    {"INS",       KEY_INSERT,     false},
    {"INSERT",    KEY_INSERT,     false},
    {"HOME",      KEY_HOME,       false},
    {"END",       KEY_END,        false},
    {"PGUP",      KEY_PAGE_UP,    false},
    {"PGDN",      KEY_PAGE_DOWN,  false},

    {"F1",  KEY_F1,   false},
    {"F2",  KEY_F2,   false},
    {"F3",  KEY_F3,   false},
    {"F4",  KEY_F4,   false},
    {"F5",  KEY_F5,   false},
    {"F6",  KEY_F6,   false},
    {"F7",  KEY_F7,   false},
    {"F8",  KEY_F8,   false},
    {"F9",  KEY_F9,   false},
    {"F10", KEY_F10,  false},
    {"F11", KEY_F11,  false},
    {"F12", KEY_F12,  false},

    // Caps 键
    {"CAPS",      KEY_CAPS_LOCK,  false},
    {"CAPSLOCK",  KEY_CAPS_LOCK,  false},

    // 修饰键自身（左右 Ctrl / Shift / Alt / Win）
    {"LCTRL",  KEY_LEFT_CTRL,   true},
    {"RCTRL",  KEY_RIGHT_CTRL,  true},
    {"LALT",   KEY_LEFT_ALT,    true},
    {"RALT",   KEY_RIGHT_ALT,   true},
    {"LSHIFT", KEY_LEFT_SHIFT,  true},
    {"RSHIFT", KEY_RIGHT_SHIFT, true},
    {"LGUI",   KEY_LEFT_GUI,    true},   // Windows / Command
    {"RGUI",   KEY_RIGHT_GUI,   true},
};

// ---------- 工具函数 ----------

// 单键（包括 F 区、方向键、ENTER、以及“单独按下的修饰键”）
static bool sendSpecialKey(const String &keyName) {
    String up = keyName;
    up.toUpperCase();

    for (auto &item : SPECIAL_KEYS) {
        if (up.equals(item.name)) {
            if (item.isModifier) {
                // 修饰键改为点按：发送一次按下+松开
                HidKeyboard.press(item.code);
                vTaskDelay(pdMS_TO_TICKS(MOD_TAP_DELAY_MS));
                HidKeyboard.release(item.code);
                vTaskDelay(pdMS_TO_TICKS(MOD_TAP_DELAY_MS));
                return true;
            } else {
                // 普通功能键 / 空格 / F 键等：一次性点击
                HidKeyboard.write(item.code);
                vTaskDelay(pdMS_TO_TICKS(KEYSTROKE_DELAY_MS));
                return false;
            }
        }
    }
    Serial.printf("Unknown special key: %s\n", keyName.c_str());
    return false;
}

// 组合键："LCTRL+C"、"LGUI+D" 之类
static void sendKeyCombo(const String &combo) {
    constexpr int MAX_PARTS = 8;

    struct Part {
        bool    isModifier = false;
        bool    isAscii    = false;
        uint8_t codeOrChar = 0;     // 修饰键/特殊键: HID 码；普通字符: ASCII
    };

    Part parts[MAX_PARTS];
    int  partCount = 0;

    int start = 0;
    while (start < combo.length() && partCount < MAX_PARTS) {
        int plus = combo.indexOf('+', start);
        int end  = (plus == -1) ? combo.length() : plus;

        String token = combo.substring(start, end);
        token.trim();
        if (token.length() > 0) {
            Part  p;
            bool  found = false;
            String up = token;
            up.toUpperCase();

            // 先按名字在 SPECIAL_KEYS 中找
            for (auto &item : SPECIAL_KEYS) {
                if (up.equals(item.name)) {
                    p.isAscii    = false;
                    p.codeOrChar = item.code;
                    p.isModifier = item.isModifier;
                    parts[partCount++] = p;
                    found = true;
                    break;
                }
            }

            // SPECIAL_KEYS 中没找到，当做单字符
            if (!found) {
                if (token.length() == 1) {
                    p.isAscii    = true;
                    p.isModifier = false;
                    p.codeOrChar = static_cast<uint8_t>(token[0]);
                    parts[partCount++] = p;
                } else {
                    Serial.printf("Unknown combo token: %s\n", token.c_str());
                }
            }
        }

        if (plus == -1) break;
        start = plus + 1;
    }

    if (partCount == 0) {
        return;
    }

    // 是否显式包含 Shift 作为修饰键
    bool hasShiftModifier = false;
    for (int i = 0; i < partCount; ++i) {
        if (parts[i].isModifier &&
            (parts[i].codeOrChar == KEY_LEFT_SHIFT ||
             parts[i].codeOrChar == KEY_RIGHT_SHIFT)) {
            hasShiftModifier = true;
            break;
        }
    }

    // 先按下所有修饰键（这里不去动 g_modState，只是临时按一下）
    for (int i = 0; i < partCount; ++i) {
        if (parts[i].isModifier) {
            HidKeyboard.press(parts[i].codeOrChar);
        }
    }

    // 再按下普通键
    for (int i = 0; i < partCount; ++i) {
        if (!parts[i].isModifier) {
            if (parts[i].isAscii) {
                uint8_t ch = parts[i].codeOrChar;

                // 如果没有显式的 Shift 修饰键，则对 A~Z 自动转小写，避免额外的 Shift
                if (!hasShiftModifier && ch >= 'A' && ch <= 'Z') {
                    ch = static_cast<uint8_t>(ch - 'A' + 'a');
                }
                HidKeyboard.press(ch);
            } else {
                HidKeyboard.press(parts[i].codeOrChar);
            }
        }
    }

    vTaskDelay(pdMS_TO_TICKS(KEYSTROKE_DELAY_MS));
    HidKeyboard.releaseAll();
}

// ---------- 初始化 & 入队 ----------

bool initKeyboardSystem() {
    // 初始化 HID
    HidKeyboard.begin();
    USB.begin();

    keyEventQueue = xQueueCreate(MAX_TEXT_QUEUE_LEN, sizeof(KeyEvent *));
    if (!keyEventQueue) {
        Serial.println("ERROR: failed to create keyEventQueue");
        return false;
    }

    BaseType_t ok = xTaskCreatePinnedToCore(
        keyboardTask,
        "KeyboardTask",
        4096,
        nullptr,
        2,
        &keyboardTaskHandle,
        1 // core 1
    );

    if (ok != pdPASS) {
        Serial.println("ERROR: failed to create KeyboardTask");
        return false;
    }
    return true;
}

static bool enqueueEvent(KeyEvent *ev) {
    if (!keyEventQueue || !ev) return false;
    if (xQueueSend(keyEventQueue, &ev, 0) != pdTRUE) {
        delete ev;
        return false;
    }
    return true;
}

bool enqueueText(const String &text) {
    if (text.isEmpty()) return true;                // 空文本视为成功但不发送
    if (text.length() > MAX_TEXT_LENGTH) return false;

    KeyEvent *ev = new KeyEvent{KeyEventType::TEXT, text};
    if (!ev) return false;
    return enqueueEvent(ev);
}

bool enqueueSpecialKeyName(const String &keyName) {
    if (keyName.isEmpty()) return false;
    KeyEvent *ev = new KeyEvent{KeyEventType::SPECIAL, keyName};
    if (!ev) return false;
    return enqueueEvent(ev);
}

bool enqueueCombo(const String &combo) {
    if (combo.isEmpty()) return false;
    KeyEvent *ev = new KeyEvent{KeyEventType::COMBO, combo};
    if (!ev) return false;
    return enqueueEvent(ev);
}

// ---------- 键盘任务：唯一直接操作 HidKeyboard 的地方 ----------

void keyboardTask(void *pvParameters) {
    (void)pvParameters;

    while (true) {
        KeyEvent *ev = nullptr;
        if (xQueueReceive(keyEventQueue, &ev, portMAX_DELAY) == pdTRUE) {
            if (!ev) continue;

            if (ev->type == KeyEventType::TEXT) {
                String &txt = ev->payload;
                for (size_t i = 0; i < txt.length(); ++i) {
                    HidKeyboard.write(static_cast<uint8_t>(txt[i]));
                    vTaskDelay(pdMS_TO_TICKS(KEYSTROKE_DELAY_MS));
                }
            } else if (ev->type == KeyEventType::SPECIAL) {
                sendSpecialKey(ev->payload);
            } else if (ev->type == KeyEventType::COMBO) {
                sendKeyCombo(ev->payload);
                // sendKeyCombo 内部已经 delay，并且 releaseAll
            }

            delete ev;
        }
    }
}
