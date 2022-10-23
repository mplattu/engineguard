#ifndef ENGINEGUARD_H
#define ENGINEGUARD_H

#include <Arduino.h>

struct EmergencyReason {
  bool isEmergency;
  String target;
  String reason;
  bool hasValue;
  String value;
};

EmergencyReason getZeroEmergencyReason();

#endif
