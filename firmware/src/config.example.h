#pragma once
//
// Copy this file to config.h in the same folder and fill in your own values.
// config.h is gitignored, so your Wi-Fi password and MQTT credentials never
// get committed.
//
//     cp config.example.h config.h   (then edit config.h)

// ---- Wi-Fi ----
#define WIFI_SSID        "YourWiFiName"
#define WIFI_PASSWORD    "YourWiFiPassword"

// ---- MQTT (HiveMQ Cloud, free tier) ----
#define MQTT_BROKER      "xxxxxxxx.s1.eu.hivemq.cloud"
#define MQTT_PORT        8883
#define MQTT_USER        "your-hivemq-username"
#define MQTT_PASS        "your-hivemq-password"
#define MQTT_CLIENT_ID   "pager-001"

#define TOPIC_ALERT      "pager/alert"
#define TOPIC_STATUS     "pager/status"

// ---- Pins (Seeed XIAO ESP32-C3) ----
#define PIN_LEDS         D0
#define PIN_MOTOR        D1
#define PIN_BUZZER       D2

// ---- Behaviour ----
#define ALERT_DURATION_MS  30000
#define BUZZ_FREQ_HZ       1000
