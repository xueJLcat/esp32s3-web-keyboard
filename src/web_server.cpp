#include "web_server.h"
#include <ArduinoJson.h>
#include "config.h"
#include "keyboard.h"
#include "web_index_html.h"

AsyncWebServer server(HTTP_PORT);

void setupHttpServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html; charset=utf-8", INDEX_HTML);
    });

    // /send 接口
    server.on(
        "/send",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            StaticJsonDocument<2048> doc;   // 留足空间
            StaticJsonDocument<256>  out;
            AsyncResponseStream *resp = request->beginResponseStream("application/json");

            DeserializationError err = deserializeJson(doc, data, len);
            if (err) {
                out["ok"]    = false;
                out["error"] = "invalid json";
                serializeJson(out, *resp);
                request->send(resp);
                return;
            }

            bool ok = false;

            if (doc.containsKey("text")) {
                String text = doc["text"].as<String>();
                if (text.length() > MAX_TEXT_LENGTH) {
                    out["ok"]    = false;
                    out["error"] = "text too long";
                } else {
                    ok = enqueueText(text);
                    out["ok"] = ok;
                    if (!ok) {
                        out["error"] = "queue full or alloc failed";
                    }
                }
            } else if (doc.containsKey("key")) {
                String key = doc["key"].as<String>();
                ok = enqueueSpecialKeyName(key);
                out["ok"] = ok;
                if (!ok) {
                    out["error"] = "queue full or alloc failed";
                }
            } else if (doc.containsKey("combo")) {
                String combo = doc["combo"].as<String>();
                ok = enqueueCombo(combo);
                out["ok"] = ok;
                if (!ok) {
                    out["error"] = "queue full or alloc failed";
                }
            } else {
                out["ok"]    = false;
                out["error"] = "no text or key or combo";
            }

            serializeJson(out, *resp);
            request->send(resp);
        });

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

    server.begin();
}
