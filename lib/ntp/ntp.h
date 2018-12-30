#ifndef NTP_H
#define NTP_H

#include <Arduino.h>
#include <WiFiUdp.h>

class Ntp {
 private:
  WiFiUDP udp;

 public:
  unsigned long unixTime();
};

#endif  // NTP_H
