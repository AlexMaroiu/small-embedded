#include <WiFi.h>
#include "config.h"
#include "DHT.h"

#define DHT22_PIN 4 //D4 pin on the esp32
DHT dht(DHT22_PIN, DHT22);

WiFiClient client;

void setup() {
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected");

  if (client.connect(serverIP, serverPort)) {
    Serial.println("Connected to server");
  } else {
    Serial.println("Connection failed");
  }
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

char buffer[100];

void loop() {
  if (client.connected()) {
    sprintf(buffer, "temperature:%.2f:humidity:%.2f", readDHTTemperature(), readDHTHumidity());
    client.println(buffer);
  }
  delay(5000);
}
