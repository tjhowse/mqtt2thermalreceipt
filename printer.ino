#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <WiFi.h>

#include "WiFiClientSecure.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "secrets.h"

#define TX_PIN 21 // Yellow to printer
#define RX_PIN 25 // Red to printer
#define BTN1_PIN 33
#define BTN2_PIN 23

// If the printer hasn't been used in this long, put it to sleep.
#define PRINTER_SLEEP_TIMEOUT_S 5

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASS);
Adafruit_MQTT_Publish boot = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC_PREFIX "/receipt_printer/boot");
Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC_PREFIX "/receipt_printer/button");
Adafruit_MQTT_Subscribe print_text = Adafruit_MQTT_Subscribe(&mqtt, MQTT_TOPIC_PREFIX "/receipt_printer/print_text");

SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);

bool btn1_debounce;
bool btn2_debounce;
bool printer_asleep;
uint16_t printer_sleepy_time;

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
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  connectWifi();
  mqtt.subscribe(&print_text);
  MQTT_connect();

  boot.publish("Hi");
  mySerial.begin(9600);
  printer.begin();
  printer_asleep = false;

  printer.setDefault();
  printer.println("Booted.");
  printer.feed(2);
  printer.sleep();
  printer_asleep = true;

  btn1_debounce = true;
  btn2_debounce = true;

  // printer.sleep();
  // delay(3000L);
  // printer.wake();
  // printer.setDefault();
}

void wake_printer_if_required() {
  if (printer_asleep) {
    Serial.println("Waking up the printer.");
    printer.wake();
    printer.setDefault();
    printer_asleep = false;
  }
}

void sleep_printer_if_required() {
  // if ((millis() > printer_sleepy_time) && !printer_asleep) {
  if (!printer_asleep) {
    printer.sleep();
    Serial.println("Putting printer to sleep.");
    printer_asleep = true;
  }
}

void schedule_printer_sleepytime() {
  // Currently unused
  return;
  printer_sleepy_time = millis() + PRINTER_SLEEP_TIMEOUT_S * 1000;
  Serial.println("Scheduling sleepytime");
}

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    // This loops until there are no more subscription notifications
    // pending from the broker.
    if (subscription == &print_text) {
      wake_printer_if_required();
      printer.println((char *)print_text.lastread);
      Serial.println((char *)print_text.lastread);
      schedule_printer_sleepytime();
    }
  }

  sleep_printer_if_required();

  if ((digitalRead(BTN1_PIN) == LOW) && btn1_debounce) {
    button.publish(1);
    Serial.println("Button 1 pressed");
    btn1_debounce = false;
  }
  if ((digitalRead(BTN2_PIN) == LOW) && btn2_debounce) {
    button.publish(2);
    Serial.println("Button 2 pressed");
    btn2_debounce = false;
  }
  if (digitalRead(BTN1_PIN) == HIGH) {
    btn1_debounce = true;
  }
  if (digitalRead(BTN2_PIN) == HIGH) {
    btn2_debounce = true;
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