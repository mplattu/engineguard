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

#include "../include/settings.cpp"

// Network
#ifdef WIFI_SSID
#define WIFI
#include <ESPmDNS.h>
#include <WiFi.h>
#endif

#ifdef MDNS_NAME
#ifdef OTA_PASSWORD
#define OTA
#include <ArduinoOTA.h>
#endif
#endif

// Display
U8X8_SSD1309_128X64_NONAME2_SW_I2C u8x8(PIN_DISPLAY_CLOCK, PIN_DISPLAY_DATA);
Display display(&u8x8);

// Onewire and sensors
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);

Sound sound(PIN_BUZZER_INTERNAL, PIN_BUZZER_EXTERNAL);

// We have two engines
Engine engine1(
  "Engine 1",              // Engine name
  "28309d1200000097",  // Engine temperature sensor 1-wire address
  31.0,                // Engine temperature emergency limit
  "28decb14000000ad",  // Room temperature sensor 1-wire address
  30.0,                // Room temperature emergency limit
  &sensors
);

Engine engine2(
  "Engine 2",
  "289cef1400000051",
  31.0,                // Engine temperature emergency limit
  "28ddd61400000064",
  30.0,                // Room temperature emergency limit
  &sensors
);

DigitalInput alarmOilEngine1("Engine 1", PIN_INPUT_RELAY_OIL_ENG_1, "Oil Pressure");
DigitalInput alarmOilEngine2("Engine 2", PIN_INPUT_RELAY_OIL_ENG_2, "Oil Pressure");
DigitalInput alarmSmoke("Engine Room", PIN_INPUT_RELAY_SMOKE, "Smoke Alarm");

// Unfortunately the interrupt-related stuff (namely the button) has to be a static
// function. So no class-stuff here!
#include "../lib/button.cpp"

#ifdef OTA
#include "../lib/ota.cpp"
#endif

void setup(void) {
  pinMode(PIN_ONBOARD_LED, OUTPUT);
  digitalWrite(PIN_ONBOARD_LED, HIGH);

  Serial.begin(115200);

  sensors.begin();

  buttonSetup(PIN_CLEAR_BUTTON, CLEAR_BUTTON_VALIDITY_IN_SECONDS);

#ifdef WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("Failed to connect to %s, rebooting in 5 seconds...", WIFI_SSID);
    delay(5000);
    Serial.println("Rebooting");
    ESP.restart();
  }

  if(!MDNS.begin(MDNS_NAME)) {
    Serial.println("Error starting mDNS");
    return;
  }
#endif

#ifdef OTA
  Serial.print("Initialising OTA...");
  initialiseArduinoOTA(MDNS_NAME, OTA_PASSWORD);
  Serial.println("OK");
#endif
}

EmergencyReason getZeroEmergencyReason(void) {
  EmergencyReason zeroEmergencyReason;

  zeroEmergencyReason.isEmergency = false;
  zeroEmergencyReason.target = "";
  zeroEmergencyReason.reason = "";
  zeroEmergencyReason.hasValue = false;
  zeroEmergencyReason.value = "";

  return zeroEmergencyReason;
}

EmergencyReason checkEmergency(void) {
  if (engine1.isEmergency()) {
    return engine1.getEmergencyReason();
  }

  if (engine2.isEmergency()) {
    return engine2.getEmergencyReason();
  }

  if (alarmOilEngine1.isEmergency()) {
    return alarmOilEngine1.getEmergencyReason();
  }

  if (alarmOilEngine2.isEmergency()) {
    return alarmOilEngine2.getEmergencyReason();
  }

  if (alarmSmoke.isEmergency()) {
    return alarmSmoke.getEmergencyReason();
  }

  return getZeroEmergencyReason();
}

void loop(void) {
  Serial.println("---");

  engine1.readSensors();
  engine2.readSensors();

  const EmergencyReason EmergencyReason = checkEmergency();

  display.updateDisplay(&engine1, &engine2, EmergencyReason);
  sound.updateSignal(EmergencyReason, buttonPressIsStillValid());

  buttonLoop();

  digitalWrite(PIN_ONBOARD_LED, LOW);
  delay(250);
  digitalWrite(PIN_ONBOARD_LED, HIGH);
  delay(250);

#ifdef WIFI
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost - attemting to reconnect");
    WiFi.disconnect();
    WiFi.reconnect();
  }
#endif

#ifdef OTA
  ArduinoOTA.handle();
#endif
}
