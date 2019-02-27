
#include <Arduino.h>

#ifdef NODE_MCU_ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef NODE_MCU_ESP32
#include <WiFi.h>
#endif

#define MAX_WIFI_RETRIES 20

#include <configuration.h>
#include <digitransit-display.h>
#include <digitransit.h>

Digitransit digitransit;
Configuration configuration;

#if DIGITRANSIT_DISPLAY == 0
LiquidCrystalDisplay display;
#else
OledDisplay display;
#endif

int wifi_connection_retries = 0;
bool wifi_configuration_mode = false;
bool wifiConnected() { return (WiFi.status() == WL_CONNECTED); }

void setup() {
  Serial.begin(115200);

  display.init();
  display.showLoadingScreen();

  Serial.println("[WIFI] connecting to wifi ...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  if (wifiConnected() && !wifi_configuration_mode) {
    Serial.println("[WIFI] connected");
    while (true) {
      if (digitransit.queryTimetable()) {
        display.clear();
        display.updateTimetable(&digitransit);
        display.showTimetable();
        delay(60000);
      } else {
        display.clear();
        display.showError();
        delay(10000);
      }
    }
    display.clear();
    display.turnOff();
  } else if (!wifi_configuration_mode) {
    Serial.println("[WIFI] not connected to wifi, still trying ...");
    delay(1000);
    wifi_connection_retries++;
    if (wifi_connection_retries >= MAX_WIFI_RETRIES) {
      WiFi.disconnect();
      wifi_configuration_mode = true;
      configuration.init();
      delay(1000);
      display.clear();
      display.showConfiguration(&configuration);
    }
  } else {
    // Configuration Async Webserver
  }
}
