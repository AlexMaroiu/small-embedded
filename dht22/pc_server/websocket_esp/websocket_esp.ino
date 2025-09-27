#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "DHT.h"

#define DHT22_PIN 4 //D4 pin on the esp32
DHT dht(DHT22_PIN, DHT22);

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_CONNECTED) {
    Serial.println("Connected to WebSocket server");
  } else if(type == WStype_DISCONNECTED) {
    Serial.println("Disconnected!");
  }
}

void setup() {
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // connect to Python server (port 5000)
  webSocket.begin("192.168.1.132", 5000, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

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

void loop() {
  webSocket.loop();

  // send data every 5 seconds
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 5000) {
    lastTime = millis();

    // build JSON
    StaticJsonDocument<200> doc;
    doc["temperature"] = readDHTTemperature();
    doc["humidity"] = readDHTHumidity();

    char buffer[128];
    serializeJson(doc, buffer);

    // send as event "esp_data"
    String msg = "42[\"esp_data\"," + String(buffer) + "]";
    webSocket.sendTXT(msg);
  }
}
