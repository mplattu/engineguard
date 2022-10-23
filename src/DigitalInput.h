#ifndef ENGINEGUARD_DIGITALINPUT_H
#define ENGINEGUARD_DIGITALINPUT_H

#include <Arduino.h>

#include "engineguard.h"

class DigitalInput {
private:
  String inputName;
  int pinInput;
  String emergencyLegend;
  EmergencyReason lastEmergencyReason;
public:
  DigitalInput(String inputName, int pinInput, String emergencyLegend);
  String getName();
  bool isEmergency();
  EmergencyReason getEmergencyReason();
};

#endif
