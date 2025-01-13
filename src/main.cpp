#include <Wire.h>
#include "SparkFunBME280.h"
#include <DHT.h>
#include <DHT_U.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define DHTPIN 4     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 22 (AM2302)

BME280 mySensor;
DHT_Unified dht(DHTPIN, DHTTYPE);

const char* ssid = "marias";
const char* password = "rvft2191";
IPAddress mqtt_server = {192, 168, 27, 36};
const int mqtt_server_port = 8884;

const char CA_cert[] = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDETCCAfmgAwIBAgIUF9HPfgvlWuCmOWvDtL1LyxL1vWEwDQYJKoZIhvcNAQEL\n" \
"BQAwGDEWMBQGA1UEAwwNVHJ1c3RlZENBLm5ldDAeFw0yNTAxMDkyMDA5MDJaFw0y\n" \
"NjAxMDkyMDA5MDJaMBgxFjAUBgNVBAMMDVRydXN0ZWRDQS5uZXQwggEiMA0GCSqG\n" \
"SIb3DQEBAQUAA4IBDwAwggEKAoIBAQC+4QVpsgBn3Fu1oLrcXuv1zuzA7b1msD+a\n" \
"TcmcsBF+bZ5XiNu++qnhq6b4fHxiUKqfB2SKT6PlZ+v/9nU5NbNxppq18wvJZIN7\n" \
"L1X5+Q31hnpx9IUrdLZLN3pepUK+ZFJWbl2ar43iKX6lG50S8vSx2lOIeYR+uUG1\n" \
"6kBGjJ4YJgHqzBtADhOARMc6um9WBrI3oGjc9lGkh47Eus2y4syar3GKrSN7zMgV\n" \
"N2Rh80EXVQ1zl+UPaIfQcjFTsdjZp3cq65nD+12UM8HuX8la9GsmxKge1EOWK18t\n" \
"kVuijnmP+gp7gqkt0t4JOGTquME646B/ptIr50CBZ0CLFZlBjtQBAgMBAAGjUzBR\n" \
"MB0GA1UdDgQWBBT5GraUvrYl3PVYlWJ1lAxnPOyUizAfBgNVHSMEGDAWgBT5GraU\n" \
"vrYl3PVYlWJ1lAxnPOyUizAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQC6z5aodJueP/Ut2owc4F5jNJmGyQtak4ViCvKcM46tkHT+zfGj6UGKe2Hw\n" \
"mWtCCzX71ox1z05z/AY2s5F4RROdEmnq+yx41g5GXBx58xETteeEHnCdDurxpR6+\n" \
"CCkTufDoEDSaNONa5Xm359XBnYJuXiCN31QzNnQd7UoL4w4r+J7CZ1yGIho0MDnW\n" \
"xL+uUFD/LShyEcGgDXTnx337CmRWyP3t1mFQN5q6A/K2CfAyc95rlAW5IWML+UvA\n" \
"QQIyCEBA6AlHZEQkyxVZ+WgQvu7yalj0I3GMp7dOug2B+c/AkcVbvNW0T97PK111\n" \
"VVHZbZNI0LVaygv6iPnAhGiouCZc\n" \
"-----END CERTIFICATE-----";

const char ESP_CA_cert[] = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIC6DCCAdCgAwIBAgIUPnreSjIKoU5A0/HgZJl4XO1cC8gwDQYJKoZIhvcNAQEL\n" \
"BQAwGDEWMBQGA1UEAwwNVHJ1c3RlZENBLm5ldDAeFw0yNTAxMDkyMDA5MDJaFw0y\n" \
"NjAxMDkyMDA5MDJaMAAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDd\n" \
"hSD5qV69FY51n4/FzJEqGHVL5WchxxfyXhbGdQH/WBkT625SJWmLx4xB97Jn/V0l\n" \
"dQuPomRzZxAGsr/eG6xTLFrvpKcFFlLX8Kzw+7NcL96I6U1HJOmsqnFubbxe4U19\n" \
"JvHc5he4AtrIcfiPMKQhG7CX5mJjnN0NaPcuD60jxt+nYwgata9hHx0mWIpYpRY3\n" \
"UE2WeTJGiUwXGEz0hn4Nzncifaqm/BVcHS+eqqUnPhtAANXm30esAdexQ05lKIk2\n" \
"Lj4fPMdB+EUtD1VaXdoILXtcdYu94ZYCAiv6pVHvSs/VPX96lPkOwkzJKX+vmiul\n" \
"RGjnTCydVf34Ro7yhULpAgMBAAGjQjBAMB0GA1UdDgQWBBRMR8a7AMbEhA9Z6rOr\n" \
"SxafhDMrazAfBgNVHSMEGDAWgBT5GraUvrYl3PVYlWJ1lAxnPOyUizANBgkqhkiG\n" \
"9w0BAQsFAAOCAQEAIrJ9XURC+mKgXQEkG7MS4FynZfcGvwREO7iPJ7mL6AVefEEO\n" \
"SBiTqqWwe7ng0rKUI19Tp5loCONqNL+91sndR5ByvWvM4oeGiKlW3aCIqH2itBxD\n" \
"G3a9+jZcor8GC83HoLjp4XRF2x8D85IB0hlkiAc5T71cng8hDLIr0ceynKBxy06Z\n" \
"ZfA7E61evNkV5tQL+iiQpla5hTLxXGPk+PQx4YuPd16JYCKVrga4+MWf5tER9JFb\n" \
"CaudS7YPqWEJgjUETfuNs9d+kPjZ1NlsxR/5luXDjt1y3hLQKTIeo10w+yO55+CF\n" \
"fU7lIlZNV8mrLu7e+kWy9j9ehxfSm7w68TNWCw==\n" \
"-----END CERTIFICATE-----";

const char ESP_RSA_key[] = \
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDdhSD5qV69FY51\n" \
"n4/FzJEqGHVL5WchxxfyXhbGdQH/WBkT625SJWmLx4xB97Jn/V0ldQuPomRzZxAG\n" \
"sr/eG6xTLFrvpKcFFlLX8Kzw+7NcL96I6U1HJOmsqnFubbxe4U19JvHc5he4AtrI\n" \
"cfiPMKQhG7CX5mJjnN0NaPcuD60jxt+nYwgata9hHx0mWIpYpRY3UE2WeTJGiUwX\n" \
"GEz0hn4Nzncifaqm/BVcHS+eqqUnPhtAANXm30esAdexQ05lKIk2Lj4fPMdB+EUt\n" \
"D1VaXdoILXtcdYu94ZYCAiv6pVHvSs/VPX96lPkOwkzJKX+vmiulRGjnTCydVf34\n" \
"Ro7yhULpAgMBAAECggEAJe7Spz38Faf31aeFsKrnnsxCZcU6RFv5+0xmXQSTW3zk\n" \
"Cnex1ApJCSEaIP/AArOtvCjPWoDF78cRxPveBDQ77aA2UDwjR6k7dXPmwXtwitQ9\n" \
"qLc+vvSQuTsUooLTE3JEWS6OkzOpkx6WBWYS2L5NFMhPlKtkZMdrSQ0U6sPPc8Uc\n" \
"X5Jk8LwZ6xFpxVy2c/D8cUx9wg61bLsFIKSmKs45E8KqRUcVi3s8ZvMRJO+NFHKq\n" \
"HdDP1RhM5BFCAITYLfT2smYQd1q7kNOmxvjATy239wrFmGBGYhvcEM/8fn4MBNFa\n" \
"1jd3mCajeRHP+s9i3bMcT/fkLeV/8PcORFrjewOfCQKBgQDuPEWCAFz9+u/Ec3mj\n" \
"X3d41g4e01I36n7GDLYtwp1ZCZd34BAQZZTiuG3P7Hbvp9A/I5Q0iOWFvXMzuf/T\n" \
"wn6EsVWnZztRy6tCvRUuuYE0s9bgU1o3Y5mlduyZEsQKGeiDSE0bJiGu9PTnM8SN\n" \
"laSnVRwozwXuQMOrFprU/kQ8JQKBgQDuCcX5RjkbcfKx4npyBxJYsy/XUGOoDsTf\n" \
"WsOZU+nau1arf2UKfFRqVDj4M3sNDxyvLFY81vmnvG/1KvgMhK/R0uxYr2ySOc/I\n" \
"0C7oNPDgTVpGbM570aK8cDYbcYLaV6DJf3ZGnY4oimR5m+7NSC/Mqzchopke7ayn\n" \
"dN5h9jDOdQKBgFDNOHYDUYcDGVd0Iwl6U7c0Qs3JttUoP59EqHWU0HFmg20xHcGx\n" \
"8nGDRBuFhEF8lXEUzCuVUuOtgelFaLOV//tZekexB7mSbp/QKeMTH/OUG4kyPCMV\n" \
"MQOh9lVkvAEBEI+WcjCHGl2Qz4+rSzsKSjJG96oPrV9FsBfTLuiW5OXRAoGAWhDO\n" \
"nJPqdaz2tEHKLKzqTy91cZWUL3lvGkfAbJpoiH+LupYYSBBk7Fpzt83ayEUoiEtR\n" \
"UfHRQps1DWG1ux7VW6HgFWpCvU0shoHmzauCXIxIFQ+ETSsz1PLsNhQL+8teJf+Z\n" \
"Y/ynVkWO4MiUYY3nsBw0XwCjySUKx82bextouiUCgYBRBag68jLR10xXdE54b+b6\n" \
"Rc4bGnBxZUJ5fHykSbCokMJc+g7OOoP8pfVlcalICmTBQzmrsxDEaiQmbZs/96JW\n" \
"K8+vcEbEGeUhxy5l4PDWy00wIu16RPX3wryP0VawaytYDNr67D04DnUI0TLoJOwP\n" \
"J+wjVgJLW/Pf7nOpdMHvxg==\n" \
"-----END PRIVATE KEY-----";

WiFiClientSecure espClient;
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
  
  espClient.setCACert(CA_cert);          //Root CA certificate
  espClient.setCertificate(ESP_CA_cert); //for client verification if the require_certificate is set to true in the mosquitto broker config
  espClient.setPrivateKey(ESP_RSA_key); 
}

void setup() {
  Serial.begin(9600);

  Wire.begin();
  Wire.setClock(400000); //Increase to fast I2C speed!

  mySensor.beginI2C();
  mySensor.setReferencePressure(101200); //Adjust the sea level pressure used for altitude calculations
  dht.begin();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_server_port);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    WiFi.mode(WIFI_STA);

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
