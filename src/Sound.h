#ifndef ENGINEGUARD_SOUND_H
#define ENGINEGUARD_SOUND_H

#include <Arduino.h>

#include "engineguard.h"

#define ENGINEGUARD_BUZZER_INT_LOW 2093
#define ENGINEGUARD_BUZZER_INT_HIGH 3136
#define ENGINEGUARD_BUZZER_CHANNEL 0

class Sound {
private:
  int pinBuzzerInternal;
  int pinBuzzerExternal;
  int toneBuzzerInternalLast;
  void localTone(int newTone);
  void on(bool suppressBuzzerExternal);
  void off();
public:
  Sound(int pinBuzzerInternal, int pinBuzzerExternal);
  void updateSignal(EmergencyReason emergencyModeReason, bool cancelInEffect);
};

#endif
