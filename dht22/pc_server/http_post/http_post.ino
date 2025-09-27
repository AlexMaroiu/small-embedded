#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"
#include "DHT.h"

#define DHT22_PIN 4 //D4 pin on the esp32
DHT dht(DHT22_PIN, DHT22);

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10          /* Time ESP32 will go to sleep (in seconds) */

float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -127;
  }
  else {
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
// Sensor readings may also be up to 2 seconds
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -127;
  }
  else {
    Serial.println(h);
    return h;
  }
}

char buffer[100];

void setup() {
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    //must be a JSON format
    sprintf(buffer, "{\"temperature\":%.2f,\"humidity\":%.2f}", readDHTTemperature(), readDHTHumidity());
    int httpResponseCode = http.POST(buffer);

    if (httpResponseCode > 0) {
      Serial.print("Response: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  Serial.flush();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start(); // send every 10 secondss
}


void loop() {
  // not needed
}