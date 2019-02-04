
#include <Arduino.h>

#ifdef NODE_MCU_ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef NODE_MCU_ESP32
#include <WiFi.h>
#endif

#include <digitransit-display.h>
#include <digitransit.h>

Digitransit digitransit;

#if DIGITRANSIT_DISPLAY == 0
LiquidCrystalDisplay display;
#else
OledDisplay display;
#endif

bool wifiConnected() { return (WiFi.status() == WL_CONNECTED); }

void setup() {
  Serial.begin(115200);

  display.init();
  display.showLoadingScreen();

  Serial.println("[WIFI] connecting to wifi ...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  if (wifiConnected()) {
    Serial.println("[WIFI] connected");
    while (true) {
      // if (digitransit.queryBikeStation()) {
      //   display.clear();
      //   display.updateTimetable(&digitransit);
      //   display.showBikeStation();
      //   delay(5000);
      // } else {
      //   display.clear();
      //   display.showError();
      //   delay(10000);
      // }
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
  } else {
    Serial.println("[WIFI] not connected to wifi, still trying ...");
    delay(1000);
  }
}

