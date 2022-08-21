#include "engineguard.h"
#include "Display.h"

Display::Display(U8X8_SSD1309_128X64_NONAME2_SW_I2C * display) {
  this->display = display;

  EmergencyModeReason emptyEmergency;
  emptyEmergency.isEmergency = false;
  emptyEmergency.target = "";
  emptyEmergency.reason = "";
  emptyEmergency.value = 0.0;

  this->lastEmergencyModeReason = emptyEmergency;

  this->heartbeatFlag = false;

  this->display->begin();
  this->setFontNormal();
}

void Display::setFontNormal() {
  this->display->setFont(u8x8_font_7x14B_1x2_f);
}

bool Display::emergencyModeReasonHasChanged(EmergencyModeReason emergencyMode) {
  if (this->lastEmergencyModeReason.isEmergency != emergencyMode.isEmergency) {
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

void Display::updateDisplay(Engine * engine1, Engine * engine2, EmergencyModeReason emergencyModeReason) {
  if (this->emergencyModeReasonHasChanged(emergencyModeReason)) {
    this->display->clearDisplay();
    this->lastEmergencyModeReason = emergencyModeReason;
  }

  if (emergencyModeReason.isEmergency) {
    this->display->drawUTF8(0, 0, "Emergency");
    this->display->drawUTF8(2, 2, emergencyModeReason.target.c_str());
    this->display->drawUTF8(2, 4, emergencyModeReason.reason.c_str());

    if (emergencyModeReason.hasValue) {
      this->display->drawUTF8(2, 6, emergencyModeReason.value.c_str());
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
