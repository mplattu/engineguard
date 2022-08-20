#include "DigitalInput.h"

DigitalInput::DigitalInput(String inputName, int pinInput) {
  this->inputName = inputName;
  this->pinInput = pinInput;

  pinMode(this->pinInput, INPUT_PULLUP);
}

String DigitalInput::getName() {
  return this->inputName;
}

bool DigitalInput::alarmIsActive() {
  return (! digitalRead(this->pinInput));
}
