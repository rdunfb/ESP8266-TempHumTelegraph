#include "Secrets.h"
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT4x.h"
#include <ESP8266WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define DEVICE "ESP8266"
#define WIFI_SSID "Zyxel_ED38-2"
#define INFLUXDB_URL "http://pi1.local:8086"
#define INFLUXDB_ORG "derungs"
#define INFLUXDB_BUCKET "sensors"
#define LOCATION "bedroom"
//#define INFLUXDB_TOKEN "MY_INFLUX_TOKEN"
//#define WIFI_PASSWORD "MY_SECRET_PASSWORD"

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

ESP8266WiFiMulti wifiMulti;
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

// Data points
Point sensor("wifi_status");
Point temperature("temperature");
Point humidity("humidity");

void setup()
{
  Wire.begin(2, 0);
  
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  
  sht4.begin();
  sht4.setPrecision(SHT4X_HIGH_PRECISION);

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(100);
  }
 
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());
  sensor.addTag("location", LOCATION);

  temperature.addTag("device", DEVICE);
  temperature.addTag("location", LOCATION);
  
  humidity.addTag("device", DEVICE);
  humidity.addTag("location", LOCATION);
  
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
}

void loop() {

  sensors_event_t hum, temp;
  sht4.getEvent(&hum, &temp);

  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();
  temperature.clearFields();
  humidity.clearFields();

  sensor.addField("rssi", WiFi.RSSI());
  temperature.addField("value", temp.temperature);
  humidity.addField("value", hum.relative_humidity);

  // If no Wifi signal, try to reconnect it
  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    //Serial.println("Wifi connection lost");
  }

  client.writePoint(temperature);
  client.writePoint(humidity);
  client.writePoint(sensor);

  // deepSleep for 15 minutes
  ESP.deepSleep(15 * 60 * 1000000);
  
  // deepSleep for 15 seconds (only for testing!)
  //ESP.deepSleep(15 * 1000000);

  // sleep for 10 seconds without deepSleep
  //delay(10000);
}
