/**
  HSL Bus Station Query
  - Queries data from HSL with GraphQL
  - Gets current timestamp via NTP
  - Display current waiting time on Serial and I2C LiquidCrystal Display
*/

#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <LiquidCrystal_I2C.h>

#include "hsl.h"

#define WIFI_SSID "***REMOVED***"
#define WIFI_PASSWORD "***REMOVED***"
#define RUNTIME_MINUTES_S 5
#define REFRESH_INTERVAL_S 30
#define SCROLL_INTERVAL_MS 500

ESP8266WiFiMulti WiFiMulti;
LiquidCrystal_I2C lcd(0x38);

Hsl hsl;

void setup() {
  Serial.begin(115200);
  lcd.begin(20, 4);
  lcd.display();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Loading ...");
  delay(1000);
  Serial.println("[WIFI] connecting to wifi ...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
  if (wifiConnected()) {
    Serial.println("[WIFI] connected");
    for (size_t i = 0; i < RUNTIME_MINUTES_S * 2; i++) {
      queryAndShowHslTimetable();
    }
    lcd.noBacklight();
    lcd.noDisplay();
    ESP.deepSleep(0);
  } else {
    Serial.println("[WIFI] not connected to wifi, still trying ...");
    delay(1000);
  }
}

void queryAndShowHslTimetable() {
  lcd.clear();
  if (hsl.queryTimetable(HSL_QUERY)) {
    lcd.setCursor(0, 1);
    Serial.printf("\n%s\n\n", hsl.station_name);
    lcd.print(hsl.station_name);
    lcd.blink();
    delay(1000);
    lcd.clear();
    displayTimetable();
  } else {
    lcd.setCursor(0, 1);
    lcd.print("HSL Query Error!");
    delay(1000);
  }
}

void displayTimetable() {
  int stringLengths[HSL_LINES];
  for (size_t i = 0; i < HSL_LINES; i++) {
    stringLengths[i] = strlen(hsl.timetable[i][1]);
  }
  int scroll_position = 0;
  
  while ((scroll_position * SCROLL_INTERVAL_MS) < (REFRESH_INTERVAL_S * 1000)) {
    Serial.println("");
    for (size_t i = 0; i < HSL_LINES; i++) {
      lcd.setCursor(0, i);
      int range = stringLengths[i] - 12;
      int offset = 0;
      if (range > 0) {
        // offset = scroll_position % (2 * range);
        // if(offset>=range){
        //   offset = range - (offset - range);
        // }
        offset = scroll_position % (range + 1);
      }
      lcd.printf("%3.3s %12.12s %3.3s", hsl.timetable[i][0], hsl.timetable[i][1] + offset, hsl.timetable[i][2]);
      Serial.printf("%3.3s %12.12s %3.3s\n", hsl.timetable[i][0], hsl.timetable[i][1] + offset, hsl.timetable[i][2]);
    }
    scroll_position++;
    delay(SCROLL_INTERVAL_MS);
  }
}

bool wifiConnected() { return (WiFiMulti.run() == WL_CONNECTED); }
