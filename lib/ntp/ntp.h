#ifndef NTP_H
#define NTP_H

#include <WiFiUdp.h>
#include <Arduino.h>

class Ntp {
private:
  WiFiUDP udp;

public:
  
  unsigned long unixTime();
};

#endif // NTP_H
