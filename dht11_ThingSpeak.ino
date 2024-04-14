#include <WiFiNINA.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "secrets.h"

#define DHTPIN 2      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // Type of DHT sensor

#define HOST "api.thingspeak.com"

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char *API_KEY = SECRET_WRITE_APIKEY;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect.
  }

  // Attempt to connect to WiFi network
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Attempting to connect to WiFi network...");
    delay(1000);
  }
  Serial.println("Connected to WiFi");

  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  if (client.connect(HOST, 80)) {
    String postStr = "api_key=";
    postStr += API_KEY;
    postStr += "&field1=";
    postStr += String(humidity);
    postStr += "&field2=";
    postStr += String(temperature);

    client.print("GET /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  } else {
    Serial.println("Connection to ThingSpeak failed!");
  }

  delay(60000); // Wait for 60 seconds before sending next data
}
