#include "Sound.h"

Sound::Sound(int pinBuzzerInternal, int pinBuzzerExternal) {
  this->pinBuzzerInternal = pinBuzzerInternal;
  this->pinBuzzerExternal = pinBuzzerExternal;

  this->toneBuzzerInternalLast = 0;

  pinMode(this->pinBuzzerInternal, OUTPUT);
  ledcSetup(ENGINEGUARD_BUZZER_CHANNEL, 0, 10);
  ledcAttachPin(this->pinBuzzerInternal, ENGINEGUARD_BUZZER_CHANNEL);

  pinMode(this->pinBuzzerExternal, OUTPUT);
}

void Sound::localTone(int newTone) {
  this->toneBuzzerInternalLast = newTone;

  ledcWriteTone(ENGINEGUARD_BUZZER_CHANNEL, newTone);
}

void Sound::updateSignal(EmergencyReason emergencyReason, bool cancelInEffect) {
  if (emergencyReason.isEmergency) {
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
