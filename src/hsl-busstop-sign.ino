/**
  HSL Bus Station Query
  - Queries data from HSL with GraphQL
  - Gets current timestamp via NTP
  - Display current waiting time on Serial and I2C LiquidCrystal Display
*/


#define WIFI_SSID "***"
#define WIFI_PASSWORD "***"

#include <Arduino.h>
#include <ESP8266WiFiMulti.h>

#include <hsl-display.h>
#include <hsl.h>

ESP8266WiFiMulti WiFiMulti;

Hsl hsl;
HslDisplay display;

void setup() {
  Serial.begin(115200);

  display.init();
  display.showLoadingScreen();

  Serial.println("[WIFI] connecting to wifi ...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  if (wifiConnected()) {
    Serial.println("[WIFI] connected");
    display.clear();
    if (hsl.queryTimetable(HSL_QUERY)) {
      while (true) {
        display.updateTimetable(&hsl);
        display.showTimetable();
        delay(60000);
      }
    } else {
      display.showError();
      delay(1000);
    }
    display.clear();
    display.turnOff();
    ESP.deepSleep(0);
  } else {
    Serial.println("[WIFI] not connected to wifi, still trying ...");
    delay(1000);
  }
}

bool wifiConnected() { return (WiFiMulti.run() == WL_CONNECTED); }
