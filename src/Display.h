#ifndef ENGINEGUARD_DISPLAY_H
#define ENGINEGUARD_DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

#include "engineguard.h"
#include "Engine.h"

class Display {
private:
  U8X8_SSD1309_128X64_NONAME2_SW_I2C * display;
  EmergencyModeReason lastEmergencyModeReason;
  bool emergencyModeReasonHasChanged(EmergencyModeReason emergencyMode);
public:
  Display(U8X8_SSD1309_128X64_NONAME2_SW_I2C * display);
  void updateDisplay(Engine * engine1, Engine * engine2, EmergencyModeReason emergencyModeReason);
};

#endif
