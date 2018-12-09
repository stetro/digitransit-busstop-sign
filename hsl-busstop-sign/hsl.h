#ifndef HSL_H
#define HSL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include "ntp.h"

#define HSL_QUERY_1                                                            \
  "{\"query\":\"{\\n  stop(id:\\\"HSL:1362130\\\") {\\n name\\n "              \
  "stoptimesWithoutPatterns(numberOfDepartures: 10) {\\n serviceDay\\n "       \
  "scheduledArrival\\n scheduledDeparture\\n trip {\\n route {\\n "            \
  "shortName\\n }\\n }\\n headsign\\n }\\n  } "                                \
  "\\n}\",\"variables\":null,\"operationName\":null}"

#define HSL_QUERY_2                                                            \
  "{\"query\":\"{\\n  stop(id:\\\"HSL:1362129\\\") {\\n name\\n "              \
  "stoptimesWithoutPatterns(numberOfDepartures: 10) {\\n serviceDay\\n "       \
  "scheduledArrival\\n scheduledDeparture\\n trip {\\n route {\\n "            \
  "shortName\\n }\\n }\\n headsign\\n }\\n  } "                                \
  "\\n}\",\"variables\":null,\"operationName\":null}"

#define HSL_URL                                                                \
  "https://api.digitransit.fi/routing/v1/routers/hsl/index/graphql"

#define HSL_FINGERPRINT                                                        \
  "8F FD 07 4E 5A 22 19 B4 75 17 45 69 59 88 7C AF 07 75 A4 B6"

#define HSL_LINES 4

class Hsl {

public:
  char station_name[20];
  char timetable[HSL_LINES][3][20];
  bool success = false;

  void queryTimetable(const char *query) {
    Serial.print("[HTTP] start query ...\n");

    // start htps query
    HTTPClient http;
    http.begin(HSL_URL, HSL_FINGERPRINT);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(query);

    if (httpCode > 0) {
      Serial.printf("[HTTP] query result code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        this->handleResponse(&http);
      }
    } else {
      Serial.printf("[HTTP] query failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }
    http.end();
  }

private:
  Ntp ntp;
  StaticJsonBuffer<4096> json_buffer;

  void handleResponse(HTTPClient *http) {
    const char *payload = http->getString().c_str();
    Serial.println("[JSON] parsing response ...");
    JsonObject &root = json_buffer.parseObject(payload);
    if (root.success()) {
      Serial.println("[JSON] success");
      long timestamp = ntp.unixTime();
      sprintf(station_name, "%19.19s",
              root["data"]["stop"]["name"].as<const char *>());
      Serial.printf("[HSL] Station: %s\n", station_name);

      JsonArray &busses = root["data"]["stop"]["stoptimesWithoutPatterns"];
      for (int i = 0; i < busses.size() && i < HSL_LINES; i++) {
        int lineDeparture = busses[i]["scheduledDeparture"];
        long lineServiceDay = busses[i]["serviceDay"];
        long departureFromNow = (lineServiceDay + lineDeparture) - timestamp;
        if (departureFromNow < 0) {
          departureFromNow = 0;
        }
        sprintf(timetable[i][0], "%3.3s",
                busses[i]["trip"]["route"]["shortName"].as<const char *>());
        sprintf(timetable[i][1], "%11.11s",
                busses[i]["headsign"].as<const char *>());
        sprintf(timetable[i][2], "%2.2dm", (departureFromNow / 60) + 1);

        Serial.printf("[HSL] %s %s %s\n", timetable[i][0], timetable[i][1],
                      timetable[i][2]);
      }
      success = true;
    } else {
      success = false;
      Serial.println("[JSON] failure");
    }
    json_buffer.clear();
  }
};

#endif // HSL_H
