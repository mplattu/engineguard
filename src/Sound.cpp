#include "Sound.h"

Sound::Sound(int pinBuzzerInternal, int pinBuzzerExternal) {
  this->pinBuzzerInternal = pinBuzzerInternal;
  this->pinBuzzerExternal = pinBuzzerExternal;

  this->toneBuzzerInternalLast = 0;

  pinMode(this->pinBuzzerExternal, OUTPUT);
}

void Sound::localTone(int newTone) {
  this->toneBuzzerInternalLast = newTone;

  if (newTone == 0) {
    noTone(this->pinBuzzerInternal);
  }
  else {
    tone(this->pinBuzzerInternal, newTone);
  }
}

void Sound::updateSignal(EmergencyModeReason emergencyModeReason, bool cancelInEffect) {
  if (emergencyModeReason.isEmergency) {
    this->on(cancelInEffect);
  }
  else {
    this->off();
  }
}

void Sound::on(bool suppressBuzzerExternal) {
  if (suppressBuzzerExternal) {
    digitalWrite(this->pinBuzzerExternal, LOW);
  }
  else {
    digitalWrite(this->pinBuzzerExternal, HIGH);
  }

  if (this->toneBuzzerInternalLast == 0 || this->toneBuzzerInternalLast == ENGINEGUARD_BUZZER_INT_LOW) {
    this->localTone(ENGINEGUARD_BUZZER_INT_HIGH);
    return;
  }

  if (this->toneBuzzerInternalLast == ENGINEGUARD_BUZZER_INT_HIGH) {
    this->localTone(ENGINEGUARD_BUZZER_INT_LOW);
    return;
  }
}

void Sound::off() {
  digitalWrite(this->pinBuzzerExternal, LOW);

  this->localTone(0);
}
