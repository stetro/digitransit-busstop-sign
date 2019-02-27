#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define CONFIGURATION_DATA_BUFFER_SIZE 30

#include <EEPROM.h>

#ifdef NODE_MCU_ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef NODE_MCU_ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#endif

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class Configuration {
 private:
  unsigned char ssid_buffer[6];
  char ssid[13];
  String ip_address;
  AsyncWebServer* server;

 public:
  void init();
  inline char* get_ssid() { return ssid; }
  inline const char* get_ip_address() { return ip_address.c_str(); }
};

struct ConfigurationData{
  char ssid[CONFIGURATION_DATA_BUFFER_SIZE];
  char password[CONFIGURATION_DATA_BUFFER_SIZE];
  char digitransit_station_id[CONFIGURATION_DATA_BUFFER_SIZE];
  char digitransit_server_id[CONFIGURATION_DATA_BUFFER_SIZE];
};

#endif  // CONFIGURATION_H
