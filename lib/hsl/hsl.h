#ifndef HSL_H
#define HSL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#include "ntp.h"

#define HSL_QUERY                                                        \
  "{\"query\":\"{\\n  stop(id:\\\"HSL:1362130\\\") {\\n name\\n "        \
  "stoptimesWithoutPatterns(numberOfDepartures: 10) {\\n serviceDay\\n " \
  "realtimeDeparture\\n trip {\\n route {\\n "      \
  "shortName\\n }\\n }\\n headsign\\n }\\n  } "                          \
  "\\n}\",\"variables\":null,\"operationName\":null}"

#define HSL_URL \
  "https://api.digitransit.fi/routing/v1/routers/hsl/index/graphql"

#define HSL_FINGERPRINT \
  "8F FD 07 4E 5A 22 19 B4 75 17 45 69 59 88 7C AF 07 75 A4 B6"

#define HSL_LINES 4

#define HSL_STRING_SIZE 40

#define STR_(X) #X

#define STR(X) STR_(X)

class Hsl {
 public:
  char station_name[HSL_STRING_SIZE + 1];
  char timetable[HSL_LINES][3][HSL_STRING_SIZE + 1];

  bool queryTimetable(const char *query);

 private:
  Ntp ntp;
  StaticJsonBuffer<4096> json_buffer;

  bool handleResponse(HTTPClient *http);
};

#endif  // HSL_H
