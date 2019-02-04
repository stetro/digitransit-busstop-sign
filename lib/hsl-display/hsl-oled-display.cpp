#include "hsl-display.h"

char display_buffer[4][40];

void OledDisplay::clear(){
  lcd->clear();
}

void OledDisplay::init() {
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

void OledDisplay::showLoadingScreen() {
  Serial.println("[Display] Show loading Screen");
  lcd->drawString(0, 0, "Loading ...");
  lcd->display();
}

void OledDisplay::turnOff() {
  Serial.println("[Display] Clear");
  lcd->clear();
}

void OledDisplay::showError() {
  Serial.println("[Display] Show Error");
  lcd->drawString(0, 0, "HSL Query Error!");
  lcd->display();
}

void OledDisplay::showTimetable() {
  Serial.println("[Display] Show Timetable");
  for (size_t i = 0; i < HSL_LINES; i++) {
    sprintf(display_buffer[i], "%3.3s %9.9s %3.3s", hsl->timetable[i][0],
            hsl->timetable[i][1] + time_table_ticker, hsl->timetable[i][2]);
    lcd->drawString(0, i * 16, display_buffer[i]);
  }
  lcd->display();
}
