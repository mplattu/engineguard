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

  this->display->begin();
  this->display->setFont(u8x8_font_7x14B_1x2_f);
}

bool Display::emergencyModeReasonHasChanged(EmergencyModeReason emergencyMode) {
  if (this->lastEmergencyModeReason.isEmergency != emergencyMode.isEmergency) {
    return true;
  }

  return false;
}

void Display::updateDisplay(Engine * engine1, Engine * engine2, EmergencyModeReason emergencyModeReason) {
  if (this->emergencyModeReasonHasChanged(emergencyModeReason)) {
    this->display->clearDisplay();
    this->lastEmergencyModeReason = emergencyModeReason;
  }

  if (emergencyModeReason.isEmergency) {
    this->display->drawUTF8(0, 0, "Emergency");
    this->display->drawUTF8(0, 2, emergencyModeReason.target.c_str());
    this->display->drawUTF8(0, 4, emergencyModeReason.reason.c_str());

    String valueStr = String(emergencyModeReason.value, 1);

    this->display->drawUTF8(0, 6, valueStr.c_str());
  }
  else {
    this->display->drawUTF8(0, 0, engine1->getName().c_str());
    this->display->drawUTF8(0, 2, engine2->getName().c_str());

    this->display->drawUTF8(5, 0, engine1->getTemperatureEngineStr().c_str());
    this->display->drawUTF8(5, 2, engine2->getTemperatureEngineStr().c_str());

    this->display->drawUTF8(10, 0, engine1->getTemperatureRoomStr().c_str());
    this->display->drawUTF8(10, 2, engine2->getTemperatureRoomStr().c_str());
  }
}
