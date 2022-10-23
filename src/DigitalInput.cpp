#include "DigitalInput.h"

DigitalInput::DigitalInput(String inputName, int pinInput, String emergencyLegend) {
  this->inputName = inputName;
  this->pinInput = pinInput;
  this->emergencyLegend = emergencyLegend;

  pinMode(this->pinInput, INPUT_PULLUP);
}

String DigitalInput::getName() {
  return this->inputName;
}

bool DigitalInput::isEmergency() {
  bool isEmergency =  (! digitalRead(this->pinInput));

  if (isEmergency) {
    this->lastEmergencyReason.isEmergency = true;
    this->lastEmergencyReason.target = this->inputName;
    this->lastEmergencyReason.reason = this->emergencyLegend;
    this->lastEmergencyReason.value = "";
    this->lastEmergencyReason.hasValue = false;
  }
  else {
    this->lastEmergencyReason = getZeroEmergencyReason();
  }

  return isEmergency;
}

EmergencyReason DigitalInput::getEmergencyReason() {
  return this->lastEmergencyReason;
}
