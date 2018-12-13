
#include "hsl.h"

bool Hsl::queryTimetable(const char *query) {
  bool success = false;
  Serial.print("[HTTP] start query ...\n");

  // start htps query
  HTTPClient http;
  http.begin(HSL_URL, HSL_FINGERPRINT);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(query);

  if (httpCode > 0) {
    Serial.printf("[HTTP] query result code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      success = this->handleResponse(&http);
    } else {
      Serial.printf("[HTTP] query failed, code %d error: %s\n", httpCode,
                    http.errorToString(httpCode).c_str());
    }
  } else {
    Serial.printf("[HTTP] query failed, error: %s\n",
                  http.errorToString(httpCode).c_str());
  }
  http.end();
  return success;
}

bool Hsl::handleResponse(HTTPClient *http) {
  bool success = false;
  const char *payload = http->getString().c_str();
  Serial.println("[JSON] parsing response ...");
  JsonObject &root = json_buffer.parseObject(payload);
  if (root.success()) {
    Serial.println("[JSON] success, parsed correctly");
    long timestamp = ntp.unixTime();
    sprintf(station_name, "%" STR(HSL_STRING_SIZE) "." STR(HSL_STRING_SIZE) "s",
            root["data"]["stop"]["name"].as<const char *>());
    Serial.printf("[HSL] Station: %s\n", station_name);

    JsonArray &busses = root["data"]["stop"]["stoptimesWithoutPatterns"];
    for (uint i = 0; i < busses.size() && i < HSL_LINES; i++) {
      int lineDeparture = busses[i]["scheduledDeparture"];
      long lineServiceDay = busses[i]["serviceDay"];
      int departureFromNow = (lineServiceDay + lineDeparture) - timestamp;
      if (departureFromNow < 0) {
        departureFromNow = 0;
      }
      sprintf(timetable[i][0], "%3.3s",
              busses[i]["trip"]["route"]["shortName"].as<const char *>());
      sprintf(timetable[i][1],
              "%" STR(HSL_STRING_SIZE) "." STR(HSL_STRING_SIZE) "s",
              busses[i]["headsign"].as<const char *>());
      sprintf(timetable[i][2], "%2.2dm", (departureFromNow / 60) + 1);
      Serial.printf(
          "[HSL] %s %" STR(HSL_STRING_SIZE) "." STR(HSL_STRING_SIZE) "s %s\n",
          timetable[i][0], timetable[i][1], timetable[i][2]);
    }
    success = true;
  } else {
    Serial.println("[JSON] could not parse response");
  }
  json_buffer.clear();
  return success;
}
