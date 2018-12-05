/**
  HSL Bus Station Query
  - Queries data from HSl viw GraphQL
  - Gets current timestamp via NTP
  - Display current waiting time on Serial (display is coming)
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include "ntp.h"

#define HSL_QUERY                                                              \
  "{\"query\":\"{\\n  stop(id:\\\"HSL:1362129\\\") {\\n name\\n "              \
  "stoptimesWithoutPatterns(numberOfDepartures: 10) {\\n serviceDay\\n "       \
  "scheduledArrival\\n scheduledDeparture\\n trip {\\n route {\\n "            \
  "shortName\\n }\\n }\\n headsign\\n }\\n  } "                                \
  "\\n}\",\"variables\":null,\"operationName\":null}"
#define HSL_URL                                                                \
  "https://api.digitransit.fi/routing/v1/routers/hsl/index/graphql"
#define HSL_FINGERPRINT                                                        \
  "8F FD 07 4E 5A 22 19 B4 75 17 45 69 59 88 7C AF 07 75 A4 B6"
#define STATIONS 4

StaticJsonBuffer<4096> jsonBuffer;
ESP8266WiFiMulti WiFiMulti;
Ntp ntp;

unsigned long inline ntpUnixTime(UDP &udp);

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
    Serial.print("[HTTP] start query ...\n");
    HTTPClient http;
    http.begin(HSL_URL, HSL_FINGERPRINT); // HTTPS
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(HSL_QUERY);

    if (httpCode > 0) {
      Serial.printf("[HTTP] query result code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        handleResponse(&http);
      }
    } else {
      Serial.printf("[HTTP] query failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }
    http.end();
    while (true) {
      delay(1000);
    }
  } else {
    Serial.println("[WIFI] not connected to wifi, retry ...");
  }

  delay(1000);
}

bool wifiConnected() { return (WiFiMulti.run() == WL_CONNECTED); }

void handleResponse(HTTPClient *http) {
  const char *payload = http->getString().c_str();
  Serial.println("[JSON] parsing response ...");
  JsonObject &root = jsonBuffer.parseObject(payload);
  if (root.success()) {
    Serial.println("[JSON] success");
    long timestamp = ntp.unixTime();
    Serial.printf("[NTP] timestamp %ld\n", timestamp);
    const char *stationName = root["data"]["stop"]["name"];
    Serial.printf("Station: %s\n", stationName);

    JsonArray &busses = root["data"]["stop"]["stoptimesWithoutPatterns"];
    for (int i = 0; i < busses.size(); i++) {
      int lineDeparture = busses[i]["scheduledDeparture"];
      long lineServiceDay = busses[i]["serviceDay"];
      long departureFromNow = (lineServiceDay + lineDeparture) - timestamp;
      if (departureFromNow < 0) {
        departureFromNow = 0;
      }
      const char *lineNumber = busses[i]["trip"]["route"]["shortName"];
      const char *lineHeadsign = busses[i]["headsign"];
      Serial.printf("%3.3s %11.11s %2.2dm\n", lineNumber, lineHeadsign,
                    (departureFromNow / 60) + 1);
      if (i >= STATIONS - 1) {
        break;
      }
    }

  } else {
    Serial.println("[JSON] failure");
  }
}
