# What?

This listens on MQTT and publishes the result to a thermal printer. It's made to run on an ESP32-based M5Stick-C connected to a QR203 thermal recept printer.

# How?
Use the arduino IDE.
Install the latest ESP32 arduino board library.
Install the Adafruit MQTT-for-esp8266/esp32 and thermal printer libaries.

Populate secrets.h:

```
const char* ssid = "yes";
const char* password = "yes";

#define MQTT_HOST "yes"
#define MQTT_PORT 1883
#define MQTT_USER "yes"
#define MQTT_PASS "yes"
#define MQTT_TOPIC_PREFIX "yes"
```