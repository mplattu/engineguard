#ifndef ENGINEGUARD_DIGITALINPUT_H
#define ENGINEGUARD_DIGITALINPUT_H

#include <Arduino.h>

class DigitalInput {
private:
  String inputName;
  int pinInput;
public:
  DigitalInput(String inputName, int pinInput);
  String getName();
  bool alarmIsActive();
};

#endif
