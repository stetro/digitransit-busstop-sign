#ifndef HSL_DISPLAY_H
#define HSL_DISPLAY_H

#ifndef HSL_DISPLAY
#define HSL_DISPLAY 1
#endif  // HSL_DISPLAY

#ifndef HSL_DISPLAY_ADDRESS
#define HSL_DISPLAY_ADDRESS 0x3C
#endif  // HSL_DISPLAY_ADDRESS

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SSD1306Wire.h>
#include <hsl.h>

#if HSL_DISPLAY == 1
#include <hsl-display-font.h>
#define PIN_WIRE_SDA 21
#define PIN_WIRE_SCL 22
#endif

class HslDisplay {
 public:
  void init();
  inline void updateTimetable(Hsl* hsl) {
    this->hsl = hsl;
    this->time_table_ticker = 0;
  };
  inline void clear() { lcd->clear(); };
  void showTimetable();
  void showLoadingScreen();
  void showError();
  void turnOff();

 private:
#if HSL_DISPLAY == 0
  LiquidCrystal_I2C* lcd;
#elif HSL_DISPLAY == 1
  SSD1306Wire* lcd;
#endif
  int time_table_ticker = 0;
  Hsl* hsl;
};

#endif  // HSL_DISPLAY_H
