#include <Wire.h>
#include "SparkFunBME280.h"
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 4     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 22 (AM2302)

BME280 mySensor;
DHT_Unified dht(DHTPIN, DHTTYPE);

const char* ssid = "marias";
const char* password = "rvft2191";
const char* mqtt_server = "192.168.27.36";

WiFiClient espClient;
PubSubClient client(espClient);
char data[1000];
long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);

  Wire.begin();
  Wire.setClock(400000); //Increase to fast I2C speed!

  mySensor.beginI2C();
  mySensor.setReferencePressure(101200); //Adjust the sea level pressure used for altitude calculations
  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  long now = millis();

  if (now - lastMsg > 5000) {
    lastMsg = now;
    float pressure = mySensor.readFloatPressure();
    Serial.print(" Pressure: ");
    Serial.print(pressure, 0);
    Serial.println();

    float temp = mySensor.readTempC();
    Serial.print(" Temp: ");
    Serial.print(temp, 2);
    Serial.println();

    sensors_event_t event;
    dht.humidity().getEvent(&event);

    Serial.print(" Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    
    memset(data, 0, 1000);
    sprintf(data, "{\"humidity\": %f, \"temperature\": %f, \"pressure\": %f}", event.relative_humidity, temp, pressure);
    client.publish("sensor/data", data);
  }
}
