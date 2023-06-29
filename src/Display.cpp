#include "engineguard.h"
#include "Display.h"

Display::Display(U8X8_SSD1309_128X64_NONAME2_SW_I2C * display) {
  this->display = display;

  this->lastEmergencyReason = getZeroEmergencyReason();

  this->heartbeatFlag = false;
  this->wifiConnected = false;

  this->display->begin();
  this->setFontNormal();
}

void Display::clearDisplay() {
  this->display->clearDisplay();
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

int Display::getMiddleLine() {
  return this->display->getRows() / 2;
}

void Display::showMessage(String message) {
  String truncatedMessage = message.substring(0, this->display->getCols());
  this->display->clearDisplay();
  this->display->drawUTF8(0, this->getMiddleLine(), truncatedMessage.c_str());
}

void Display::updateDisplayHeartbeat() {
  if (this->heartbeatFlag) {
    if (this->wifiConnected) {
      this->display->setFont(u8x8_font_open_iconic_embedded_2x2);
      this->display->drawUTF8(this->getIconColumn(), 0, "P");
    }
    else {
      this->display->setFont(u8x8_font_open_iconic_weather_2x2);
      this->display->drawUTF8(this->getIconColumn(), 0, "E");
    }
  }
  else {
    this->display->setFont(u8x8_font_open_iconic_weather_2x2);
    this->display->drawUTF8(this->getIconColumn(), 0, " ");
  }

  this->setFontNormal();

  this->heartbeatFlag = ! this->heartbeatFlag;
}

void Display::clearObsoleteEmergencyReason(EmergencyReason emergencyReason) {
  if (this->emergencyReasonHasChanged(emergencyReason)) {
    this->display->clearDisplay();
    this->lastEmergencyReason = emergencyReason;
  }
}

void Display::updateDisplayEmergencyMode(EmergencyReason emergencyReason) {
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

void Display::updateDisplay(EmergencyReason emergencyReason, bool wifiConnected) {
  this->wifiConnected = wifiConnected;
  this->clearObsoleteEmergencyReason(emergencyReason);

  if (emergencyReason.isEmergency) {
    this->updateDisplayEmergencyMode(emergencyReason);
  }
  else {
    this->updateDisplayHeartbeat();
  }
}

void Display::updateDisplay(Engine * engine, EmergencyReason emergencyReason, bool wifiConnected) {
  this->wifiConnected = wifiConnected;
  this->clearObsoleteEmergencyReason(emergencyReason);

  if (emergencyReason.isEmergency) {
    this->updateDisplayEmergencyMode(emergencyReason);
  }
  else {
    this->display->drawUTF8(0, 1, engine->getName().c_str());
    this->display->drawUTF8(1, 3, engine->getTemperatureEngineStr().c_str());
    this->display->drawUTF8(8, 3, engine->getTemperatureRoomStr().c_str());

    this->updateDisplayHeartbeat();
  }
}

void Display::updateDisplay(Engine * engine1, Engine * engine2, EmergencyReason emergencyReason, bool wifiConnected) {
  this->wifiConnected = wifiConnected;
  this->clearObsoleteEmergencyReason(emergencyReason);

  if (emergencyReason.isEmergency) {
    this->updateDisplayEmergencyMode(emergencyReason);
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

void Display::showEmergencyLimits(Engine * engine) {
  this->display->clearDisplay();

  this->display->drawUTF8(0, 1, engine->getName().c_str());
  this->display->drawUTF8(1, 3, engine->getEmergencyLimitEngineStr().c_str());
  this->display->drawUTF8(8, 3, engine->getEmergencyLimitRoomStr().c_str());
}

void Display::showEmergencyLimits(Engine * engine1, Engine * engine2) {
  this->display->clearDisplay();

  this->display->drawUTF8(0, 0, engine1->getName().c_str());
  this->display->drawUTF8(1, 2, engine1->getEmergencyLimitEngineStr().c_str());
  this->display->drawUTF8(8, 2, engine1->getEmergencyLimitRoomStr().c_str());

  this->display->drawUTF8(0, 4, engine2->getName().c_str());
  this->display->drawUTF8(1, 6, engine2->getEmergencyLimitEngineStr().c_str());
  this->display->drawUTF8(8, 6, engine2->getEmergencyLimitRoomStr().c_str());
}