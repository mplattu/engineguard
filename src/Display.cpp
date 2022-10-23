#include "engineguard.h"
#include "Display.h"

Display::Display(U8X8_SSD1309_128X64_NONAME2_SW_I2C * display) {
  this->display = display;

  this->lastEmergencyReason = getZeroEmergencyReason();

  this->heartbeatFlag = false;

  this->display->begin();
  this->setFontNormal();
}

void Display::setFontNormal() {
  this->display->setFont(u8x8_font_7x14B_1x2_f);
}

bool Display::emergencyReasonHasChanged(EmergencyReason emergencyMode) {
  if (this->lastEmergencyReason.isEmergency != emergencyMode.isEmergency) {
    return true;
  }

  return false;
}

int Display::getIconColumn() {
  return this->display->getCols()-3;
}

void Display::updateDisplayHeartbeat() {
  this->display->setFont(u8x8_font_open_iconic_weather_2x2);

  if (this->heartbeatFlag) {
    this->display->drawUTF8(this->getIconColumn(), 0, "E");
  }
  else {
    this->display->drawUTF8(this->getIconColumn(), 0, " ");
  }

  this->setFontNormal();

  this->heartbeatFlag = ! this->heartbeatFlag;
}

void Display::updateDisplay(Engine * engine1, Engine * engine2, EmergencyReason emergencyReason) {
  if (this->emergencyReasonHasChanged(emergencyReason)) {
    this->display->clearDisplay();
    this->lastEmergencyReason = emergencyReason;
  }

  if (emergencyReason.isEmergency) {
    this->display->drawUTF8(0, 0, "Emergency");
    this->display->drawUTF8(2, 2, emergencyReason.target.c_str());
    this->display->drawUTF8(2, 4, emergencyReason.reason.c_str());

    if (emergencyReason.hasValue) {
      this->display->drawUTF8(2, 6, emergencyReason.value.c_str());
    }

    this->display->setFont(u8x8_font_open_iconic_embedded_2x2);
    this->display->drawUTF8(this->getIconColumn(), 0, "C");
    this->setFontNormal();

  }
  else {
    this->display->drawUTF8(0, 0, engine1->getName().c_str());
    this->display->drawUTF8(1, 2, engine1->getTemperatureEngineStr().c_str());
    this->display->drawUTF8(8, 2, engine1->getTemperatureRoomStr().c_str());

    this->display->drawUTF8(0, 4, engine2->getName().c_str());
    this->display->drawUTF8(1, 6, engine2->getTemperatureEngineStr().c_str());
    this->display->drawUTF8(8, 6, engine2->getTemperatureRoomStr().c_str());

    this->updateDisplayHeartbeat();
  }

}
