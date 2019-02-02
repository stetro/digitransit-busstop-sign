
#include "hsl.h"

bool Hsl::queryTimetable() {
  bool success = false;
  Serial.print("[HTTP] start query ...\n");

  // start htps query
  HTTPClient http;
#ifdef NODE_MCU_ESP32
  http.begin(HSL_URL, HSL_CERTIFICATE);
#else
  http.begin(HSL_URL, HSL_FINGERPRINT);
#endif
  http.addHeader("Content-Type", "application/json");
  String query = String(HSL_QUERY);

  query.replace("HSL_STATION_ID", HSL_STATION_ID);
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
    while (timestamp == 0) {
      timestamp = ntp.unixTime();
      delay(100);
    }

    sprintf(station_name, "%." STR(HSL_STRING_SIZE) "s",
            root["data"]["stop"]["name"].as<const char *>());
    this->utf8ascii(station_name);
    Serial.printf("[HSL] Station: %s\n", station_name);

    JsonArray &busses = root["data"]["stop"]["stoptimesWithoutPatterns"];
    for (uint i = 0; i < busses.size() && i < HSL_LINES; i++) {
      int lineDeparture = busses[i]["realtimeDeparture"];
      long lineServiceDay = busses[i]["serviceDay"];
      int departureFromNow = (lineServiceDay + lineDeparture) - timestamp;
      if (departureFromNow < 0) {
        departureFromNow = 0;
      }
      sprintf(timetable[i][0], "%3.3s",
              busses[i]["trip"]["route"]["shortName"].as<const char *>());
      sprintf(timetable[i][1], "%." STR(HSL_STRING_SIZE) "s",
              busses[i]["headsign"].as<const char *>());
      sprintf(timetable[i][2], "%2.2dm", (departureFromNow / 60) + 1);
      this->utf8ascii(timetable[i][1]);
      Serial.printf(
          "[HSL] %s %-" STR(HSL_STRING_SIZE) "." STR(HSL_STRING_SIZE) "s %s\n",
          timetable[i][0], timetable[i][1], timetable[i][2]);
    }
    success = true;
  } else {
    Serial.println("[JSON] could not parse response");
  }
  json_buffer.clear();
  return success;
}

// ****** UTF8-Decoder: convert UTF8-string to extended ASCII *******

// Convert a single Character from UTF8 to Extended ASCII
// Return "0" if a byte has to be ignored
byte Hsl::utf8ascii(byte ascii) {
  if (ascii < 128)  // Standard ASCII-set 0..0x7F handling
  {
    c1 = 0;
    return (ascii);
  }

  // get previous input
  byte last = c1;  // get last char
  c1 = ascii;      // remember actual character

  switch (last)  // conversion depending on first UTF8-character
  {
    case 0xC2:
      return (ascii);
      break;
    case 0xC3:
      return (ascii | 0xC0);
      break;
    case 0x82:
      if (ascii == 0xAC) return (0x80);  // special case Euro-symbol
  }

  return (0);  // otherwise: return zero, if character has to be ignored
}

// convert String object from UTF8 String to Extended ASCII
String Hsl::utf8ascii(String s) {
  String r = "";
  char c;
  for (int i = 0; i < s.length(); i++) {
    c = this->utf8ascii(s.charAt(i));
    if (c != 0) r += c;
  }
  return r;
}

// In Place conversion UTF8-string to Extended ASCII (ASCII is shorter!)
void Hsl::utf8ascii(char *s) {
  int k = 0;
  char c;
  for (int i = 0; i < strlen(s); i++) {
    c = this->utf8ascii(s[i]);
    if (c != 0) s[k++] = c;
  }
  s[k] = 0;
}