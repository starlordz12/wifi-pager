// Wi-Fi Pager — V1 firmware (Seeed XIAO ESP32-C3)
//
// Subscribes to an MQTT topic; when a "page" arrives it flashes the LEDs,
// buzzes the vibration motor, and chimes the buzzer for ALERT_DURATION_MS.
// See ../wifi-pager-design.md for the full system design and wiring.
//
// Copy src/config.example.h -> src/config.h and fill in your values first.

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <esp_wifi.h>          // esp_wifi_set_ps() — modem-sleep power saving
#include "config.h"

WiFiClientSecure wifiClient;
PubSubClient mqtt(wifiClient);

// Flash LEDs + buzz motor + chime buzzer for ALERT_DURATION_MS.
void runAlert() {
  unsigned long start = millis();
  bool on = false;
  while (millis() - start < ALERT_DURATION_MS) {
    on = !on;
    digitalWrite(PIN_LEDS,  on ? HIGH : LOW);
    digitalWrite(PIN_MOTOR, on ? HIGH : LOW);
    if (millis() - start < 3000) tone(PIN_BUZZER, BUZZ_FREQ_HZ, 400);
    delay(500);
  }
  digitalWrite(PIN_LEDS,  LOW);
  digitalWrite(PIN_MOTOR, LOW);
  noTone(PIN_BUZZER);
}

void onMqttMessage(char* /*topic*/, byte* /*payload*/, unsigned int /*len*/) {
  Serial.println("[PAGER] Alert received!");
  runAlert();
  mqtt.publish("pager/ack", "ok");
}

void connectWifi() {
  Serial.printf("[WIFI] Connecting to %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.printf("\n[WIFI] Connected. IP: %s\n", WiFi.localIP().toString().c_str());
}

void connectMqtt() {
  wifiClient.setInsecure();   // V1: skip TLS cert validation (hobby). See cloud/README.md.
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMqttMessage);
  Serial.print("[MQTT] Connecting...");
  while (!mqtt.connected()) {
    if (mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println(" connected.");
      mqtt.subscribe(TOPIC_ALERT, 1);
      mqtt.publish(TOPIC_STATUS, "online");
    } else {
      Serial.printf(" failed (rc=%d), retry in 5s\n", mqtt.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LEDS,   OUTPUT);
  pinMode(PIN_MOTOR,  OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Power-on self-test: everything fires once so you know the wiring is good.
  digitalWrite(PIN_LEDS, HIGH); digitalWrite(PIN_MOTOR, HIGH);
  tone(PIN_BUZZER, 880, 200); delay(300);
  digitalWrite(PIN_LEDS, LOW); digitalWrite(PIN_MOTOR, LOW);

  connectWifi();
  esp_wifi_set_ps(WIFI_PS_MIN_MODEM);   // modem sleep, ~3-5mA average
  connectMqtt();
  Serial.println("[READY] Waiting for pages...");
}

void loop() {
  if (!mqtt.connected()) connectMqtt();
  mqtt.loop();
}
