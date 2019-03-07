
#include <Arduino.h>

#ifdef NODE_MCU_ESP8266
#include <ESP8266WiFi.h>
#define ENDLESS_DEEPSLEEP 0
#endif

#ifdef NODE_MCU_ESP32
#include <WiFi.h>
#define ENDLESS_DEEPSLEEP -1
#endif

#define MAX_WIFI_RETRIES 20

#include <configuration.h>
#include <digitransit-display.h>
#include <digitransit.h>

Digitransit digitransit;
Configuration configuration;
ConfigurationData* configuration_data;

#if DIGITRANSIT_DISPLAY == 0
LiquidCrystalDisplay display;
#else
OledDisplay display;
#endif

int show_counter = 0;
int wifi_connection_retries = 0;
bool wifi_configuration_mode = false;
bool wifiConnected() { return (WiFi.status() == WL_CONNECTED); }

void deepSleep() {
  display.turnOff();
  Serial.println("[ESP] deep sleep");
  delay(1000);
  ESP.deepSleep(ENDLESS_DEEPSLEEP);
}

void setup() {
  Serial.begin(115200);

  display.init();
  display.showLoadingScreen();

  configuration_data = configuration.get_configuration();
  if (!(configuration_data->eeprom_check[0] == 'O' &&
        configuration_data->eeprom_check[1] == 'K')) {
    WiFi.disconnect();
    wifi_configuration_mode = true;
    configuration.init();
    delay(1000);
    display.clear();
    display.showConfiguration(&configuration);
    return;
  }

  Serial.println("[WIFI] connecting to wifi ...");
  WiFi.begin(configuration_data->ssid, configuration_data->password);
}

void loop() {
  if (Serial.available() > 0) {
    if (Serial.read() == 'c') {
      configuration.clear();
    }
  }
  if (wifiConnected() && !wifi_configuration_mode) {
    Serial.println("[WIFI] connected");
    while (true) {
      bool querySucceeded = false;
      if (configuration_data->bike_station) {
        querySucceeded = digitransit.queryBikeStation(
            configuration_data->digitransit_server_id,
            configuration_data->digitransit_station_id);
      } else {
        querySucceeded = digitransit.queryTimetable(
            configuration_data->digitransit_server_id,
            configuration_data->digitransit_station_id);
      }

      if (querySucceeded) {
        display.clear();
        display.updateTimetable(&digitransit);
        if (configuration_data->bike_station) {
          display.showBikeStation();
        } else {
          display.showTimetable();
        }
        delay(60000);
        show_counter++;
        if (show_counter >= configuration_data->turnoff &&
            configuration_data->turnoff > 0) {
          deepSleep();
        }
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
