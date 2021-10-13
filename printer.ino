#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <WiFi.h>

#include "WiFiClientSecure.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "secrets.h"

#define TX_PIN 26
#define RX_PIN 36
#define BTN_PIN 39

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASS);
Adafruit_MQTT_Publish boot = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC_PREFIX "/receipt_printer/boot");
Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC_PREFIX "/receipt_printer/button");
Adafruit_MQTT_Subscribe print_text = Adafruit_MQTT_Subscribe(&mqtt, MQTT_TOPIC_PREFIX "/receipt_printer/print_text");

SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);

bool btn_debounce;

void connectWifi() {

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  connectWifi();
  mqtt.subscribe(&print_text);
  MQTT_connect();

  boot.publish("Hi");
  mySerial.begin(9600);
  printer.begin();

  printer.setDefault();
  printer.println("Booted.");
  printer.feed(2);

  btn_debounce = true;

  // printer.sleep();
  // delay(3000L);
  // printer.wake();
  // printer.setDefault();
}

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(50))) {
    if (subscription == &print_text) {
      printer.println((char *)print_text.lastread);
    }
  }
  if ((digitalRead(BTN_PIN) == LOW) && btn_debounce) {
    Serial.println("Button pressed");
    button.publish(1);
    btn_debounce = false;
  }
  if (digitalRead(BTN_PIN) == HIGH) {
    btn_debounce = true;
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}