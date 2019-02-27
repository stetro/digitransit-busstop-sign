#include <configuration-site.h>
#include <configuration.h>

void Configuration::init() {
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
    Serial.println("[Configuration] password " + password);
    sprintf(data.password, "%s", password.c_str());

    String station = request->arg("station");
    Serial.println("[Configuration] station " + station);
    sprintf(data.digitransit_station_id, "%s", station.c_str());

    String server = request->arg("server");
    Serial.println("[Configuration] server " + server);
    sprintf(data.digitransit_server_id, "%s", server.c_str());

    EEPROM.put(0, data);
    EEPROM.commit();

    request->send(200, "text/html",
                  "Configuration updated. Restart your device.");
  });

  server->begin();

  Serial.printf("[Configuration] Running Soft AP at %s\n",
                WiFi.softAPIP().toString().c_str());
}