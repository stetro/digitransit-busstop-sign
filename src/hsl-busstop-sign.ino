/**
  HSL Bus Station Query
  - Queries data from HSl viw GraphQL
  - Gets current timestamp via NTP
  - Display current waiting time on Serial (display is coming)
*/

#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <LiquidCrystal_I2C.h>

#include "hsl.h"

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

ESP8266WiFiMulti WiFiMulti;
LiquidCrystal_I2C lcd(0x38);

Hsl hsl;

void setup() {
  lcd.begin(20, 4);
  Serial.begin(115200);
  delay(1000);
  Serial.println("[WIFI] connecting to wifi ...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  if (wifiConnected()) {
    Serial.println("[WIFI] connected");
    if (hsl.queryTimetable(HSL_QUERY)) {
      // TODO: Display Data on LCD
      // hsl.station_name
      // hsl.timetable
    } else {
    }
    delay(30 * 1000);
  } else {
    Serial.println("[WIFI] not connected to wifi, still trying ...");
    delay(1000);
  }
}

bool wifiConnected() { return (WiFiMulti.run() == WL_CONNECTED); }
