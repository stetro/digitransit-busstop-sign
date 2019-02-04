#ifndef HSL_DISPLAY_H
#define HSL_DISPLAY_H


#ifndef HSL_DISPLAY_ADDRESS
#define HSL_DISPLAY_ADDRESS 0x3C
#endif  // HSL_DISPLAY_ADDRESS

#ifdef NODE_MCU_ESP32
#define PIN_WIRE_SDA 21
#define PIN_WIRE_SCL 22
#endif

#include <Wire.h>
#include <hsl.h>

// Liquid Crystal includes
#include <LiquidCrystal_I2C.h>

// OLED Display includes
#include <SSD1306Wire.h>
#include <hsl-display-font.h>

class HslDisplay {
 public:
  inline void updateTimetable(Hsl* hsl) {
    this->hsl = hsl;
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
  Hsl* hsl;
};

class OledDisplay : public HslDisplay {
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

class LiquidCrystalDisplay : public HslDisplay {
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
#endif  // HSL_DISPLAY_H
