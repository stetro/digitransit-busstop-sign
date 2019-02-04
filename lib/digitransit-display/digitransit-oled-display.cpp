#include "digitransit-display.h"

char display_buffer[4][40];

void OledDisplay::clear() { lcd->clear(); }

void OledDisplay::init() {
  Wire.begin();
  Serial.printf("[Display] Connecting to OLED 0x%02x\n",
                DIGITRANSIT_DISPLAY_ADDRESS);
  lcd =
      new SSD1306Wire(DIGITRANSIT_DISPLAY_ADDRESS, PIN_WIRE_SDA, PIN_WIRE_SCL);
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
  lcd->drawString(0, 0, "Query Error!");
  lcd->display();
}

void OledDisplay::showTimetable() {
  Serial.println("[Display] Show Timetable");
  for (size_t i = 0; i < DIGITRANSIT_LINES; i++) {
    sprintf(display_buffer[i], "%3.3s %9.9s %3.3s",
            digitransit->timetable[i][0],
            digitransit->timetable[i][1] + time_table_ticker,
            digitransit->timetable[i][2]);
    lcd->drawString(0, i * 16, display_buffer[i]);
  }
  lcd->display();
}

void OledDisplay::showBikeStation() {
  Serial.println("[Display] Bike Station");
  lcd->drawString(0, 0, digitransit->station_name);
  lcd->drawString(0, 32, digitransit->bike_station[0]);
  lcd->drawString(0, 48, digitransit->bike_station[1]);
  lcd->display();
}
