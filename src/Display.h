#ifndef ENGINEGUARD_DISPLAY_H
#define ENGINEGUARD_DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <WiFi.h>

#include "engineguard.h"
#include "Engine.h"

class Display {
private:
  U8X8_SSD1309_128X64_NONAME2_SW_I2C * display;
  EmergencyReason lastEmergencyReason;
  int getIconColumn();
  int getMiddleLine();
  void setFontNormal();
  bool emergencyReasonHasChanged(EmergencyReason emergencyMode);
  void updateDisplayHeartbeat();
  bool heartbeatFlag;
  bool wifiConnected;
  void clearObsoleteEmergencyReason(EmergencyReason emergencyReason);
  void updateDisplayEmergencyMode(EmergencyReason emergencyReason);
public:
  Display(U8X8_SSD1309_128X64_NONAME2_SW_I2C * display);
  void clearDisplay();
  void showMessage(String message);
  void updateDisplay(EmergencyReason emergencyReason, bool wifiConnected);
  void updateDisplay(Engine * engine, EmergencyReason emergencyReason, bool wifiConnected);
  void updateDisplay(Engine * engine1, Engine * engine2, EmergencyReason emergencyReason, bool wifiConnected);
};

#endif
