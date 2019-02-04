#ifndef DIGITRANSIT_DISPLAY_H
#define DIGITRANSIT_DISPLAY_H


#ifndef DIGITRANSIT_DISPLAY_ADDRESS
#define DIGITRANSIT_DISPLAY_ADDRESS 0x3C
#endif  // DIGITRANSIT_DISPLAY_ADDRESS

#ifdef NODE_MCU_ESP32
#define PIN_WIRE_SDA 21
#define PIN_WIRE_SCL 22
#endif

#include <Wire.h>
#include <digitransit.h>

// Liquid Crystal includes
#include <LiquidCrystal_I2C.h>

// OLED Display includes
#include <SSD1306Wire.h>
#include <digitransit-display-font.h>

class DigitransitDisplay {
 public:
  inline void updateTimetable(Digitransit* digitransit) {
    this->digitransit = digitransit;
    this->time_table_ticker = 0;
  };

  virtual void init() = 0;
  virtual void clear() = 0;
  virtual void showTimetable() = 0;
  virtual void showLoadingScreen() = 0;
  virtual void showError() = 0;
  virtual void turnOff() = 0;

 protected:
  int time_table_ticker = 0;
  Digitransit* digitransit;
};

class OledDisplay : public DigitransitDisplay {
 public:
  void init();
  void clear();
  void showTimetable();
  void showLoadingScreen();
  void showError();
  void turnOff();

 private:
  SSD1306Wire* lcd;
};

class LiquidCrystalDisplay : public DigitransitDisplay {
 public:
  void init();
  void clear();
  void showTimetable();
  void showLoadingScreen();
  void showError();
  void turnOff();

 private:
  LiquidCrystal_I2C* lcd;
};
#endif  // DIGITRANSIT_DISPLAY_H
