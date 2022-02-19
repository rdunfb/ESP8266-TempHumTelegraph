# ESP8266-TempHumTelegraph

Simple Arduino Sketch for reading Temperature & Humidity from SHT4X Sensor and uploading to a Influx database.

# Requirements

- InfluxDB Server (I'm using a Raspberry Pi 3B+ with Raspberry Pi OS 64 Bit + Docker)
- SHT4X Temperature and Humidity Sensor (Using SHT40)
- ESP8266 (Using ESP-01 https://www.instructables.com/Getting-Started-With-the-ESP8266-ESP-01/)
- ESP8266 Programmer
- Arduino IDE

# Arduino Libraries
- Adafruit SHT4X Library
- ArduinoJson
- ESP8266 Influxdb
- Adafruit Unified Sensor (not sure if needed)
- Adafruit BusIO (not sure if needed, maybe dependency)

# InfluxDB using Docker

Start a InfluxDB instance using:
```
docker run -d -p 8086:8086 --name influxdb --restart always -v $PWD/data:/var/lib/influxdb2 -v $PWD/config:/etc/influxdb2 -e DOCKER_INFLUXDB_INIT_MODE=setup -e DOCKER_INFLUXDB_INIT_USERNAME=influx -e DOCKER_INFLUXDB_INIT_PASSWORD=my-secret-password -e DOCKER_INFLUXDB_INIT_ORG=my-org-name -e DOCKER_INFLUXDB_INIT_BUCKET=sensors influxdb:2.0
```
Be sure to edit username, password, organization name, bucket name and maybe --restart policy and container name.

# Set up Raspberry as Wifi Access Point (Optional)

Follow official Raspberry Pi instructions (https://www.raspberrypi.com/documentation/computers/configuration.html#setting-up-a-bridged-wireless-access-point)

**WARNING**

If you run Docker + Wifi Access Point on the same Pi then you need to make some network changes:

Source: https://forums.raspberrypi.com/viewtopic.php?t=287804#p1741377

Run
```
sudo iptables -A FORWARD -i br0 -j ACCEPT
```

Then install package `iptables-persistent`

```
sudo apt update
sudo apt install iptables-persistent
```

Copy over settings:
```
sudo iptables-save | sudo tee /etc/iptables/rules.v4
```


# Modify / Update Arduino Sketch

Clone repository and add a file called "Secrets.h" and add these two lines:
```
#define INFLUXDB_TOKEN "MY_INFLUX_TOKEN"
#define WIFI_PASSWORD "MY_SECRET_PASSWORD"
```
alternatively remove `#include "Secrets.h"` from .ino File and uncomment said lines and hard-code credentials for Wifi and Influx token. Just don't commit and push them...

Update defines `WIFI_SSID`, `INFLUXDB_URL`, `INFLUXDB_ORG`, `INFLUXDB_BUCKET`, `LOCATION` and maybe `TZ_INFO`
Update line `ESP.deepSleep(15 * 60 * 1000000)` to any number you like. Replace 15 * 60 with the amounts of seconds to sleep (15 * 60 = 15 Minutes) or remove line completely and add a delay if you don't want to use deep sleep (but consumes a lot of power.. not recommended for battery driven)

# Wiring
Connect SHT40 to ESP 3.3V, GND and SCL to GPIO2 and SDA to GPIO0 (or the other way around? Not 100% sure..)

Pinouts: https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

# Power
I use 2x 1.5V AA batteries in series for a total of ~3V. Since the ESP8266 consumes a lot of power I recommend using the built-in `deepSleep` method. Unfortunately the ESP-01 can't wake up by itself using the default configuration. You need to add a wire:

Check out https://quadmeup.com/esp8266-esp-01-low-power-mode-run-it-for-months/

According to the author it should run for abount 5 months which is pretty good considering it's high power usage.

