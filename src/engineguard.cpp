#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Engine.h"
#include "Display.h"
#include "Sound.h"

#define PIN_DISPLAY_CLOCK 22
#define PIN_DISPLAY_DATA 21
#define PIN_ONEWIRE 4
#define PIN_BUZZER_INTERNAL 18
#define PIN_BUZZER_EXTERNAL 19
#define PIN_CLEAR_BUTTON 23
#define CLEAR_BUTTON_VALIDITY_IN_SECONDS 30

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

// Unfortunately the interrupt-related stuff (namely the button) has to be a static
// function. So no class-stuff here!
#include "../lib/button.cpp"

void setup(void) {
  Serial.begin(115200);

  sensors.begin();

  buttonSetup(PIN_CLEAR_BUTTON, CLEAR_BUTTON_VALIDITY_IN_SECONDS);
}

EmergencyModeReason checkEmergency(void) {
  EmergencyModeReason reason;
  reason.isEmergency = false;
  reason.target = "";
  reason.reason = "";
  reason.value = 0.0;

  if (engine1.isEmergencyEngine()) {
    reason.isEmergency = true;
    reason.target = engine1.getName();
    reason.reason = "Engine Temp";
    reason.value = engine1.getTemperatureEngine();

    return reason;
  }

  if (engine1.isEmergencyRoom()) {
    reason.isEmergency = true;
    reason.target = engine1.getName();
    reason.reason = "Room Temp";
    reason.value = engine1.getTemperatureRoom();

    return reason;
  }

  if (engine2.isEmergencyEngine()) {
    reason.isEmergency = true;
    reason.target = engine2.getName();
    reason.reason = "Engine Temp";
    reason.value = engine2.getTemperatureEngine();

    return reason;
  }

  if (engine2.isEmergencyRoom()) {
    reason.isEmergency = true;
    reason.target = engine2.getName();
    reason.reason = "Room Temp";
    reason.value = engine2.getTemperatureRoom();

    return reason;
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

  delay(1000);
}
