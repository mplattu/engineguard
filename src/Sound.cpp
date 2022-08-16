#include "Sound.h"

Sound::Sound(int pinBuzzerInternal) {
  this->pinBuzzerInternal = pinBuzzerInternal;
  this->toneBuzzerInternalLast = 0;
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

void Sound::updateSignal(EmergencyModeReason emergencyModeReason) {
  if (emergencyModeReason.isEmergency) {
    this->on();
  }
  else {
    this->off();
  }
}

void Sound::on() {
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
  this->localTone(0);
}
