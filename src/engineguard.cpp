#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Engine.h"
#include "DigitalInput.h"
#include "Display.h"
#include "Sound.h"

#define PIN_ONBOARD_LED  2
#define PIN_DISPLAY_CLOCK 22
#define PIN_DISPLAY_DATA 21
#define PIN_ONEWIRE 4
#define PIN_BUZZER_INTERNAL 18
#define PIN_BUZZER_EXTERNAL 19
#define PIN_CLEAR_BUTTON 23
#define CLEAR_BUTTON_VALIDITY_IN_SECONDS 30
#define PIN_INPUT_RELAY_OIL_ENG_1 25
#define PIN_INPUT_RELAY_OIL_ENG_2 32
#define PIN_INPUT_RELAY_SMOKE 33

// Display
U8X8_SSD1309_128X64_NONAME2_SW_I2C u8x8(PIN_DISPLAY_CLOCK, PIN_DISPLAY_DATA);
Display display(&u8x8);

// Onewire and sensors
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);

Sound sound(PIN_BUZZER_INTERNAL, PIN_BUZZER_EXTERNAL);

// We have two engines
Engine engine1(
  "Eng1",              // Engine name
  "28309d1200000097",  // Engine temperature sensor 1-wire address
  31.0,                // Engine temperature emergency limit
  "28decb14000000ad",  // Room temperature sensor 1-wire address
  30.0,                // Room temperature emergency limit
  &sensors
);

Engine engine2(
  "Eng2",
  "289cef1400000051",
  31.0,                // Engine temperature emergency limit
  "28ddd61400000064",
  30.0,                // Room temperature emergency limit
  &sensors
);

DigitalInput alarmOilEngine1("Eng1", PIN_INPUT_RELAY_OIL_ENG_1);
DigitalInput alarmOilEngine2("Eng2", PIN_INPUT_RELAY_OIL_ENG_2);
DigitalInput alarmSmoke("Eng Room", PIN_INPUT_RELAY_SMOKE);

// Unfortunately the interrupt-related stuff (namely the button) has to be a static
// function. So no class-stuff here!
#include "../lib/button.cpp"

void setup(void) {
  pinMode(PIN_ONBOARD_LED, OUTPUT);
  digitalWrite(PIN_ONBOARD_LED, HIGH);

  Serial.begin(115200);

  sensors.begin();

  buttonSetup(PIN_CLEAR_BUTTON, CLEAR_BUTTON_VALIDITY_IN_SECONDS);
}

EmergencyModeReason checkEmergency(void) {
  EmergencyModeReason reason;
  reason.isEmergency = false;
  reason.target = "";
  reason.reason = "";
  reason.hasValue = false;
  reason.value = 0.0;

  if (engine1.isEmergencyEngine()) {
    reason.isEmergency = true;
    reason.target = engine1.getName();
    reason.reason = "Engine Temp";
    reason.value = engine1.getTemperatureEngine();
    reason.hasValue = true;

    return reason;
  }

  if (engine1.isEmergencyRoom()) {
    reason.isEmergency = true;
    reason.target = engine1.getName();
    reason.reason = "Room Temp";
    reason.value = engine1.getTemperatureRoom();
    reason.hasValue = true;

    return reason;
  }

  if (engine2.isEmergencyEngine()) {
    reason.isEmergency = true;
    reason.target = engine2.getName();
    reason.reason = "Engine Temp";
    reason.value = engine2.getTemperatureEngine();
    reason.hasValue = true;

    return reason;
  }

  if (engine2.isEmergencyRoom()) {
    reason.isEmergency = true;
    reason.target = engine2.getName();
    reason.reason = "Room Temp";
    reason.value = engine2.getTemperatureRoom();
    reason.hasValue = true;

    return reason;
  }

  if (alarmOilEngine1.alarmIsActive()) {
    reason.isEmergency = true;
    reason.target = alarmOilEngine1.getName();
    reason.reason = "Oil Pressure";
    reason.hasValue = false;
  }

  if (alarmOilEngine2.alarmIsActive()) {
    reason.isEmergency = true;
    reason.target = alarmOilEngine2.getName();
    reason.reason = "Oil Pressure";
    reason.hasValue = false;
  }

  if (alarmSmoke.alarmIsActive()) {
    reason.isEmergency = true;
    reason.target = alarmSmoke.getName();
    reason.reason = "Smoke Alarm";
    reason.hasValue = false;
  }

  return reason;
}

void loop(void) {
  Serial.println("---");

  engine1.readSensors();
  engine2.readSensors();

  const EmergencyModeReason emergencyModeReason = checkEmergency();

  display.updateDisplay(&engine1, &engine2, emergencyModeReason);
  sound.updateSignal(emergencyModeReason, buttonPressIsStillValid());

  buttonLoop();

  digitalWrite(PIN_ONBOARD_LED, LOW);
  delay(500);
  digitalWrite(PIN_ONBOARD_LED, HIGH);
  delay(500);
}
