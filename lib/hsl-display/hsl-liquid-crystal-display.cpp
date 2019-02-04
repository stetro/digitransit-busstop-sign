#include "hsl-display.h"

void LiquidCrystalDisplay::clear(){
  lcd->clear();
}

void LiquidCrystalDisplay::init() {
  Serial.printf("[Display] Connecting to Liquid Display 0x%02x\n",
                HSL_DISPLAY_ADDRESS);
  lcd = new LiquidCrystal_I2C(HSL_DISPLAY_ADDRESS, 20, 4);
  lcd->init();
  lcd->noCursor();
  lcd->display();
  lcd->backlight();
  lcd->clear();
  lcd->setCursor(1, 1);
}

void LiquidCrystalDisplay::showLoadingScreen() {
  Serial.println("[Display] Show loading Screen");
  lcd->setCursor(0, 1);
  lcd->print("Loading ...");
}

void LiquidCrystalDisplay::turnOff() {
  Serial.println("[Display] Turn Off");
  lcd->noBacklight();
  lcd->noDisplay();
}

void LiquidCrystalDisplay::showError() {
  Serial.println("[Display] Show Error");
  lcd->setCursor(0, 1);
  lcd->print("HSL Query Error!");
}

void LiquidCrystalDisplay::showTimetable() {
  Serial.println("[Display] Show Timetable");
  for (size_t i = 0; i < HSL_LINES; i++) {
    lcd->printf("%3.3s %12.12s %3.3s", hsl->timetable[i][0],
                hsl->timetable[i][1] + time_table_ticker, hsl->timetable[i][2]);
  }
}
