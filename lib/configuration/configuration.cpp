#include <configuration-site.h>
#include <configuration.h>

void Configuration::init() {
  WiFi.begin();
  WiFi.macAddress(ssid_buffer);
  sprintf(ssid, "%02X%02X%02X%02X%02X%02X", ssid_buffer[0], ssid_buffer[1],
          ssid_buffer[2], ssid_buffer[3], ssid_buffer[4], ssid_buffer[5]);
  WiFi.softAP(ssid, NULL);
  ip_address = WiFi.softAPIP().toString();

  server = new AsyncWebServer(80);

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("[Configuration] loading index.html");
    request->send(200, "text/html", INDEX_HTML);
  });

  server->on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[Configuration] updating configuration");

    EEPROM.begin(512);

    ConfigurationData data;

    String ssid = request->arg("ssid");
    Serial.println("[Configuration] ssid " + ssid);
    sprintf(data.ssid, "%s", ssid.c_str());

    String password = request->arg("password");
    Serial.println("[Configuration] password ***");
    sprintf(data.password, "%s", password.c_str());

    String station = request->arg("station");
    Serial.println("[Configuration] station " + station);
    sprintf(data.digitransit_station_id, "%s", station.c_str());

    String server = request->arg("server");
    Serial.println("[Configuration] server " + server);
    sprintf(data.digitransit_server_id, "%s", server.c_str());

    String station_type = request->arg("station_type");
    Serial.println("[Configuration] station type " + station_type);
    data.bike_station = station_type.equals("bike");

    String turnoff = request->arg("turnoff");
    Serial.println("[Configuration] station type " + turnoff);
    data.turnoff = turnoff.toInt();

    sprintf(data.eeprom_check, "OK");

    EEPROM.put(0, data);
    EEPROM.commit();

    request->send(200, "text/html",
                  "Configuration updated. Restart your device.");
  });

  server->begin();

  Serial.printf("[Configuration] Running Soft AP at %s\n",
                WiFi.softAPIP().toString().c_str());
}

ConfigurationData *Configuration::get_configuration() {
  Serial.println("[Configuration] loading configuration");
  EEPROM.begin(512);
  EEPROM.get(0, configuration_data);

  Serial.printf("[Configuration] EEPROM %s \n",
                configuration_data.eeprom_check);
  Serial.printf("[Configuration] ssid %s \n", configuration_data.ssid);
  Serial.printf("[Configuration] password ***\n");
  Serial.printf("[Configuration] server %s \n",
                configuration_data.digitransit_server_id);
  Serial.printf("[Configuration] station %s \n",
                configuration_data.digitransit_station_id);
  Serial.printf("[Configuration] station type %s \n",
                configuration_data.bike_station ? "Bike" : "Bus");
  Serial.printf("[Configuration] turn off minutes %d \n",
                configuration_data.turnoff);

  return &configuration_data;
}

void Configuration::clear() {
  Serial.println("[Configuration] clear configuration");
  EEPROM.begin(512);
  ConfigurationData data;
  sprintf(data.eeprom_check, "**");
  sprintf(data.ssid, "***");
  sprintf(data.password, "***");
  sprintf(data.digitransit_server_id, "***");
  sprintf(data.digitransit_station_id, "***");
  data.bike_station = false;
  data.turnoff = 0;
  EEPROM.put(0, data);
  EEPROM.commit();
}