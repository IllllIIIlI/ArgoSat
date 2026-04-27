#include <Arduino.h>
#include <ArduinoJson.h>

// Kintamieji
int   co2         = 0;
float temperatura = 0;
float dregme      = 0;
float lat         = 0;
float lon         = 0;

void setup() {
    Serial.begin(115200);   // Debug
    Serial1.begin(115200, SERIAL_8N1, 44, 43);  // RX=44, TX=43 pagal schemą
    Serial.println("ESP32 paruoštas");
}

void loop() {
    // Laukti duomenų iš Raspberry
    if (Serial1.available()) {
        String eilute = Serial1.readStringUntil('\n');

        // Analizuoti JSON
        StaticJsonDocument<256> doc;
        DeserializationError klaida = deserializeJson(doc, eilute);

        if (!klaida) {
            co2         = doc["co2"]         | 0;
            temperatura = doc["temperatura"] | 0.0;
            dregme      = doc["dregme"]      | 0.0;
            lat         = doc["lat"]         | 0.0;
            lon         = doc["lon"]         | 0.0;

            // Spausdinti gautus duomenis
            Serial.println("--- Gauta iš Raspberry ---");
            Serial.printf("CO2:         %d ppm\n",  co2);
            Serial.printf("Temperatūra: %.1f °C\n", temperatura);
            Serial.printf("Drėgmė:      %.1f %%\n", dregme);
            Serial.printf("GPS:         %.6f, %.6f\n", lat, lon);
        } else {
            Serial.println("JSON klaida!");
        }
    }
}
```

---

### Įdiegti ESP32 biblioteką:

Arduino IDE -> Library Manager:
```
ArduinoJson