# hsl-busstop-sign
[![Build Status](https://travis-ci.com/stetro/hsl-busstop-sign.svg?branch=master)](https://travis-ci.com/stetro/hsl-busstop-sign)
HSL (Helsingin Seudun Liikenne) busstop sign based on ESP8266 or ESP32

  - Queries data from HSL with GraphQL
  - Gets current timestamp via NTP
  - Display current waiting time on Serial and I2C Display
    - I2C OLED display supported
    - 20x4 Liquid Crystal display supported

![](demo.jpg)

```
[WIFI] connecting to wifi ...
[WIFI] not connected to wifi, retry ...
[WIFI] not connected to wifi, retry ...
[WIFI] connected

[HTTP] start query ...
[HTTP] query result code: 200

[JSON] parsing response ...
[JSON] success

[NTP] query current time ...
[NTP] timestamp 1544374441

[HSL] Station:    Erik Spåren tie
[HSL] 506 Meilahti vi 03m
[HSL]  78 Rautatiento 05m
[HSL]  79 Herttoniemi 11m
[HSL]  57 Munkkiniemi 16m
```

![](hsl-busstop-sign.png)

## Usage

How to build your own HSL Busstop Sign:

1. Find your station ID after search in the address bar of https://reittiopas.hsl.fi
2. Set all needed variables in `platformio.ini`
  - `hsl_station_id` - Graph QL HSL Station ID
  - `display` - `0` for Liquid Crystal Display and `1` for OLED Display
  - `display_address` - I2C display address
  - `wifi_ssid` and `widi_password`
3. Install PlatformIO and flash NodeMCU
  - `pip install -U platformio`
  - `platformio update`
  - `platformio run --target upload`

### Partlist

For the basic usage only the NodeMcu and LCD display is needed.

* NodeMCU ESP32 or NodeMCU ESP8266
* LCD Display 20x4 with I2C Adapter or OLED I2C Display
* [Tactile Push Button]
* [10k Resistor]
* [Lithium Ion Battery 3.7]
* [5V stepup board with charging circuit]

