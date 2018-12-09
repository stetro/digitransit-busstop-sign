/**
  HSL Bus Station Query
  - Queries data from HSl viw GraphQL
  - Gets current timestamp via NTP
  - Display current waiting time on Serial (display is coming)
*/

#include <Arduino.h>
#include <ESP8266WiFiMulti.h>

#include "hsl.h"

ESP8266WiFiMulti WiFiMulti;
Hsl hsl;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("[WIFI] connecting to wifi ...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("***", "***");
}

void loop() {
  if (wifiConnected()) {
    Serial.println("[WIFI] connected");

    hsl.queryTimetable(HSL_QUERY_1);
    if (hsl.success) {
      hsl.timetable;
    } else {
    }
    delay(30 * 1000);
    hsl.queryTimetable(HSL_QUERY_2);
    delay(30 * 1000);

  } else {
    Serial.println("[WIFI] not connected to wifi, retry ...");
  }

  delay(1000);
}

bool wifiConnected() { return (WiFiMulti.run() == WL_CONNECTED); }
