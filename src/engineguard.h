#ifndef ENGINEGUARD_H
#define ENGINEGUARD_H

#include <Arduino.h>

struct EmergencyModeReason {
  bool isEmergency;
  String target;
  String reason;
  float value;
};

#endif
