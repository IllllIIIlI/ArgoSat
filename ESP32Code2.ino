#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include <WiFi.h>

// ⚠️ Įrašyk ground station ESP32 MAC adresą
uint8_t groundStationMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Duomenų struktūra
typedef struct {
    int   co2;
    float temperatura;
    float dregme;
    float lat;
    float lon;
} SensorData;

SensorData duomenys;
esp_now_peer_info_t peerInfo;

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Siuntimo statusas: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "KLAIDA");
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 44, 43);

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW klaida!");
        return;
    }

    esp_now_register_send_cb(onSent);

    memcpy(peerInfo.peer_addr, groundStationMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    Serial.println("ESP32 paruoštas");
}

void loop() {
    if (Serial1.available()) {
        String eilute = Serial1.readStringUntil('\n');
        eilute.trim();

        JsonDocument doc;
        DeserializationError klaida = deserializeJson(doc, eilute);

        if (!klaida) {
            duomenys.co2         = doc["co2"]         | 0;
            duomenys.temperatura = doc["temperatura"] | 0.0;
            duomenys.dregme      = doc["dregme"]      | 0.0;
            duomenys.lat         = doc["lat"]         | 0.0;
            duomenys.lon         = doc["lon"]         | 0.0;

            esp_now_send(groundStationMAC, (uint8_t *)&duomenys, sizeof(duomenys));

            Serial.printf("Siunciama: CO2=%d, T=%.1f, D=%.1f, GPS=%.6f,%.6f\n",
                duomenys.co2, duomenys.temperatura, duomenys.dregme,
                duomenys.lat, duomenys.lon);
        } else {
            Serial.println("JSON klaida!");
        }
    }
}