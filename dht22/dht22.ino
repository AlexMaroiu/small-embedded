/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-dht22
 */

#include "DHT.h"

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>

#define DHT22_PIN 4

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#endif

DHT dht22(DHT22_PIN, DHT22);

void setup() {
  Serial.begin(9600);
  dht22.begin(); // initialize the DHT22 sensor
}

void loop() {
  // wait a few seconds between measurements.
  delay(2000);

  // read humidity
  float humi  = dht22.readHumidity();
  // read temperature as Celsius
  float tempC = dht22.readTemperature();
  // read temperature as Fahrenheit
  float tempF = dht22.readTemperature(true);

  // check if any reads failed
  if (isnan(humi) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT22 sensor!");
  } else {
    Serial.print("DHT22# Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  "); 

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C ~ ");
    Serial.print(tempF);
    Serial.println("°F");
    Serial.println(dht22.computeHeatIndex(tempC, humi, false));
  }
}
