/** HSL Bus Station Query */

#include <Arduino.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define HSL_QUERY "{\"query\":\"{\\n  stop(id:\\\"HSL:1362129\\\") {\\n name\\n stoptimesWithoutPatterns(numberOfDepartures: 10) {\\n serviceDay\\n scheduledArrival\\n scheduledDeparture\\n trip {\\n route {\\n shortName\\n }\\n }\\n headsign\\n }\\n  } \\n}\",\"variables\":null,\"operationName\":null}"
#define HSL_URL "https://api.digitransit.fi/routing/v1/routers/hsl/index/graphql"
#define HSL_FINGERPRINT "8F FD 07 4E 5A 22 19 B4 75 17 45 69 59 88 7C AF 07 75 A4 B6"
#define STATIONS 4

StaticJsonBuffer<4096> jsonBuffer;
ESP8266WiFiMulti WiFiMulti;
WiFiUDP udp;

unsigned long inline ntpUnixTime (UDP &udp);

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
    http.begin(HSL_URL, HSL_FINGERPRINT); //HTTPS
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(HSL_QUERY);

    if (httpCode > 0) {
      Serial.printf("[HTTP] query result code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        handleResponse(&http);
      }
    } else {
      Serial.printf("[HTTP] query failed, error: %s\n", http.errorToString(httpCode).c_str());
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

bool wifiConnected() {
  return (WiFiMulti.run() == WL_CONNECTED);
}

void handleResponse(HTTPClient *http) {
  const char* payload = http->getString().c_str();
  Serial.println("[JSON] parsing response ...");
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (root.success()) {
    Serial.println("[JSON] success");
    long timestamp = ntpUnixTime(udp);
    Serial.printf("[NTP] timestamp %ld\n", timestamp);
    const char* stationName = root["data"]["stop"]["name"];
    Serial.printf("Station: %s\n", stationName);

    JsonArray& busses = root["data"]["stop"]["stoptimesWithoutPatterns"];
    for (int i = 0; i < busses.size(); i++) {
      int lineDeparture = busses[i]["scheduledDeparture"];
      long lineServiceDay = busses[i]["serviceDay"];
      long departureFromNow = (lineServiceDay + lineDeparture) - timestamp;
      if (departureFromNow < 0) {
        departureFromNow = 0;
      }
      const char* lineNumber = busses[i]["trip"]["route"]["shortName"];
      const char* lineHeadsign = busses[i]["headsign"];
      Serial.printf("%3.3s %11.11s %2.2dm\n", lineNumber, lineHeadsign, (departureFromNow / 60) + 1);
      if (i >= STATIONS - 1) {
        break;
      }
    }

  } else {
    Serial.println("[JSON] failure");
  }
}

unsigned long inline ntpUnixTime (UDP &udp)
{
  static int udpInited = udp.begin(123); // open socket on arbitrary port

  const char timeServer[] = "pool.ntp.org";  // NTP server

  // Only the first four bytes of an outgoing NTP packet need to be set
  // appropriately, the rest can be whatever.
  const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header

  // Fail if WiFiUdp.begin() could not init a socket
  if (! udpInited)
    return 0;

  // Clear received data from possible stray received packets
  udp.flush();

  // Send an NTP request
  if (! (udp.beginPacket(timeServer, 123) // 123 is the NTP port
         && udp.write((byte *)&ntpFirstFourBytes, 48) == 48
         && udp.endPacket()))
    return 0;       // sending request failed

  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 150;   // poll every this many ms
  const byte maxPoll = 15;    // poll up to this many times
  int pktLen;       // received packet length
  for (byte i = 0; i < maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == 48)
      break;
    delay(pollIntv);
  }
  if (pktLen != 48)
    return 0;       // no correct packet received

  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  // Read the integer part of sending time
  unsigned long time = udp.read();  // NTP time
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  // Round to the nearest second if we want accuracy
  // The fractionary part is the next byte divided by 256: if it is
  // greater than 500ms we round to the next second; we also account
  // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
  // additionally, we account for how much we delayed reading the packet
  // since its arrival, which we assume on average to be pollIntv/2.
  time += (udp.read() > 115 - pollIntv / 8);

  // Discard the rest of the packet
  udp.flush();

  return time - 2208988800ul;   // convert NTP time to Unix time
}
