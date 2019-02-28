
#include "digitransit.h"

bool Digitransit::queryTimetable(const char * server_id, const char * station_id) {
  bool success = false;
  Serial.print("[HTTP] start query ...\n");

  // start htps query
  HTTPClient http;
  String url = DIGITRANSIT_URL;
  url.replace("DIGITRANSIT_SERVER_ID", server_id);
#ifdef NODE_MCU_ESP32
  http.begin(url, DIGITRANSIT_CERTIFICATE);
#else
  http.begin(url, DIGITRANSIT_FINGERPRINT);
#endif
  http.addHeader("Content-Type", "application/json");
  String query = String(DIGITRANSIT_QUERY);

  query.replace("DIGITRANSIT_STATION_ID", station_id);
  int httpCode = http.POST(query);

  if (httpCode > 0) {
    Serial.printf("[HTTP] query result code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      success = this->handleTimetableResponse(&http);
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

bool Digitransit::handleTimetableResponse(HTTPClient *http) {
  bool success = false;
  const char *payload = http->getString().c_str();
  Serial.println("[JSON] parsing response ...");
  JsonObject &root = json_buffer.parseObject(payload);
  if (root.success()) {
    Serial.println("[JSON] success, parsed correctly");
    long timestamp = ntp.unixTime();
    while (timestamp == 0) {
      timestamp = ntp.unixTime();
      delay(100);
    }

    sprintf(station_name, "%." STR(DIGITRANSIT_STRING_SIZE) "s",
            root["data"]["stop"]["name"].as<const char *>());
    Serial.printf("[DIGITRANSIT] Station: %s\n", station_name);

    JsonArray &busses = root["data"]["stop"]["stoptimesWithoutPatterns"];
    for (uint i = 0; i < busses.size() && i < DIGITRANSIT_LINES; i++) {
      int lineDeparture = busses[i]["realtimeDeparture"];
      long lineServiceDay = busses[i]["serviceDay"];
      int departureFromNow = (lineServiceDay + lineDeparture) - timestamp;
      if (departureFromNow < 0) {
        departureFromNow = 0;
      }
      sprintf(timetable[i][0], "%3.3s",
              busses[i]["trip"]["route"]["shortName"].as<const char *>());
      sprintf(timetable[i][1], "%." STR(DIGITRANSIT_STRING_SIZE) "s",
              busses[i]["headsign"].as<const char *>());
      sprintf(timetable[i][2], "%2.2dm", (departureFromNow / 60) + 1);
      Serial.printf("[DIGITRANSIT] %s %-" STR(DIGITRANSIT_STRING_SIZE) "." STR(
                        DIGITRANSIT_STRING_SIZE) "s %s\n",
                    timetable[i][0], timetable[i][1], timetable[i][2]);
    }
    success = true;
  } else {
    Serial.println("[JSON] could not parse response");
  }
  json_buffer.clear();
  return success;
}

bool Digitransit::queryBikeStation(const char * server_id, const char * station_id) {
  bool success = false;
  Serial.print("[HTTP] start bike station query ...\n");

  // start htps query
  HTTPClient http;
  String url = DIGITRANSIT_URL;
  url.replace("DIGITRANSIT_SERVER_ID", server_id);
#ifdef NODE_MCU_ESP32
  http.begin(url, DIGITRANSIT_CERTIFICATE);
#else
  http.begin(url, DIGITRANSIT_FINGERPRINT);
#endif
  http.addHeader("Content-Type", "application/json");
  String query = String(DIGITRANSIT_BUS_STATION_QUERY);

  query.replace("DIGITRANSIT_BIKE_STATION_ID", station_id);
  int httpCode = http.POST(query);

  if (httpCode > 0) {
    Serial.printf("[HTTP] query result code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      success = this->handleBusStationResponse(&http);
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

bool Digitransit::handleBusStationResponse(HTTPClient *http) {
  bool success = false;
  const char *payload = http->getString().c_str();
  Serial.println("[JSON] parsing response ... ");
  JsonObject &root = json_buffer.parseObject(payload);
  if (root.success()) {
    Serial.println("[JSON] success, parsed correctly");

    sprintf(station_name, "%." STR(DIGITRANSIT_STRING_SIZE) "s",
            root["data"]["bikeRentalStation"]["name"].as<const char *>());
    sprintf(bike_station[0], "%d / %d",
            root["data"]["bikeRentalStation"]["bikesAvailable"].as<int>(),
            root["data"]["bikeRentalStation"]["spacesAvailable"].as<int>());
    sprintf(bike_station[1], "%." STR(DIGITRANSIT_STRING_SIZE) "s",
            root["data"]["bikeRentalStation"]["state"].as<const char *>());
    Serial.printf("[DIGITRANSIT] Station: %s\n", station_name);
    Serial.printf("[DIGITRANSIT] %." STR(DIGITRANSIT_STRING_SIZE) "s \n",
                  bike_station[0]);
    Serial.printf("[DIGITRANSIT] %." STR(DIGITRANSIT_STRING_SIZE) "s \n",
                  bike_station[1]);
    success = true;
  } else {
    Serial.println("[JSON] could not parse response");
  }
  json_buffer.clear();
  return success;
}