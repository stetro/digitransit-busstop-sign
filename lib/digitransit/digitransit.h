#ifndef DIGITRANSIT_H
#define DIGITRANSIT_H

#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef NODE_MCU_ESP8266
#include <ESP8266HTTPClient.h>
#endif

#ifdef NODE_MCU_ESP32
#include <HTTPClient.h>
#endif

#include "ntp.h"

#define DIGITRANSIT_BUS_STATION_QUERY                                       \
  "{\"query\":\"{\\n "                                                      \
  "bikeRentalStation(id:\\\"DIGITRANSIT_BIKE_STATION_ID\\\") {\\n name\\n " \
  "bikesAvailable\\n state\\n spacesAvailable\\n }\\n "                     \
  "}\\n\",\"variables\":null,\"operationName\":null}"

#define DIGITRANSIT_QUERY                                                    \
  "{\"query\":\"{\\n  stop(id:\\\"DIGITRANSIT_STATION_ID\\\") {\\n name\\n " \
  "stoptimesWithoutPatterns(numberOfDepartures: 10) {\\n serviceDay\\n "     \
  "realtimeDeparture\\n trip {\\n route {\\n "                               \
  "shortName\\n }\\n }\\n headsign\\n }\\n  } "                              \
  "\\n}\",\"variables\":null,\"operationName\":null}"

#define DIGITRANSIT_URL                                                     \
  "https://api.digitransit.fi:443/routing/v1/routers/DIGITRANSIT_ID/index/" \
  "graphql"

#define DIGITRANSIT_FINGERPRINT \
  "8F FD 07 4E 5A 22 19 B4 75 17 45 69 59 88 7C AF 07 75 A4 B6"

#define DIGITRANSIT_CERTIFICATE                                        \
  "-----BEGIN CERTIFICATE-----\n"                                      \
  "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
  "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
  "ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
  "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
  "LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
  "RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
  "+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
  "PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
  "xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
  "Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
  "hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
  "EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
  "MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
  "FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
  "nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
  "eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
  "hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
  "Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
  "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
  "+OkuE6N36B9K\n"                                                     \
  "-----END CERTIFICATE-----\n"

#define DIGITRANSIT_LINES 4

#define DIGITRANSIT_STRING_SIZE 40

#define STR_(X) #X

#define STR(X) STR_(X)

class Digitransit {
 public:
  char station_name[DIGITRANSIT_STRING_SIZE + 1];
  char timetable[DIGITRANSIT_LINES][3][DIGITRANSIT_STRING_SIZE + 1];
  char bike_station[2][DIGITRANSIT_STRING_SIZE + 1];

  bool queryTimetable();
  bool queryBikeStation();

 private:
  Ntp ntp;
  StaticJsonBuffer<4096> json_buffer;

  bool handleTimetableResponse(HTTPClient *http);
  bool handleBusStationResponse(HTTPClient *http);
};

#endif  // DIGITRANSIT_H
