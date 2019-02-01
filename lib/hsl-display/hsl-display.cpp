#include "hsl-display.h"

#if HSL_DISPLAY == 0

void HslDisplay::init() {
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

void HslDisplay::showLoadingScreen() {
  Serial.println("[Display] Show loading Screen");
  lcd->setCursor(0, 1);
  lcd->print("Loading ...");
}

void HslDisplay::turnOff() {
  Serial.println("[Display] Turn Off");
  lcd->noBacklight();
  lcd->noDisplay();
}

void HslDisplay::showError() {
  Serial.println("[Display] Show Error");
  lcd->setCursor(0, 1);
  lcd->print("HSL Query Error!");
}

void HslDisplay::showTimetable() {
  Serial.println("[Display] Show Timetable");
  for (size_t i = 0; i < HSL_LINES; i++) {
    lcd->printf("%3.3s %12.12s %3.3s", hsl->timetable[i][0],
                hsl->timetable[i][1] + time_table_ticker, hsl->timetable[i][2]);
  }
}

#elif HSL_DISPLAY == 1

char display_buffer[4][40];

void HslDisplay::init() {
  Wire.begin();
  Serial.printf("[Display] Connecting to OLED 0x%02x\n", HSL_DISPLAY_ADDRESS);
  lcd = new SSD1306Wire(HSL_DISPLAY_ADDRESS, PIN_WIRE_SDA, PIN_WIRE_SCL);
  lcd->init();
  lcd->display();

  lcd->flipScreenVertically();
  lcd->setFont(Monospaced_plain_12);
  lcd->setTextAlignment(TEXT_ALIGN_LEFT);

  lcd->clear();
}

void HslDisplay::showLoadingScreen() {
  Serial.println("[Display] Show loading Screen");
  lcd->drawString(0, 0, "Loading ...");
  lcd->display();
}

void HslDisplay::turnOff() {
  Serial.println("[Display] Clear");
  lcd->clear();
}

void HslDisplay::showError() {
  Serial.println("[Display] Show Error");
  lcd->drawString(0, 0, "HSL Query Error!");
  lcd->display();
}

void HslDisplay::showTimetable() {
  Serial.println("[Display] Show Timetable");
  for (size_t i = 0; i < HSL_LINES; i++) {
    sprintf(display_buffer[i], "%3.3s %9.9s %3.3s", hsl->timetable[i][0],
            hsl->timetable[i][1] + time_table_ticker, hsl->timetable[i][2]);
    lcd->drawString(0, i * 16, display_buffer[i]);
  }
  lcd->display();
}

#endif