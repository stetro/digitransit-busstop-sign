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

struct ConfigurationData{
  char eeprom_check[3];
  char ssid[CONFIGURATION_DATA_BUFFER_SIZE];
  char password[CONFIGURATION_DATA_BUFFER_SIZE];
  char digitransit_station_id[CONFIGURATION_DATA_BUFFER_SIZE];
  char digitransit_server_id[CONFIGURATION_DATA_BUFFER_SIZE];
  bool bike_station = false;
  int turnoff = 0;
};

class Configuration {
 private:
  unsigned char ssid_buffer[6];
  char ssid[13];
  String ip_address;
  AsyncWebServer* server;
  ConfigurationData configuration_data;

 public:
  void init();
  inline char* get_ssid() { return ssid; }
  inline const char* get_ip_address() { return ip_address.c_str(); }
  ConfigurationData* get_configuration();
  void clear();
};


#endif  // CONFIGURATION_H
