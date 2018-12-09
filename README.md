# hsl-busstop-sign
HSL (Helsingin Seudun Liikenne) busstop sign based on NodeMCU

  - Queries data from HSl viw GraphQL
  - Gets current timestamp via NTP
  - Display current waiting time on Serial (display is coming)

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
