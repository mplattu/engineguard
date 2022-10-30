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
#include <WebServer.h>
WebServer webServer(80);
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

// Engines
#ifdef ENGINE1_NAME
#define ENGINE1
Engine engine1(
  ENGINE1_NAME,             // Engine name
  ENGINE1_TEMP_ENG,         // Engine temperature sensor 1-wire address
  ENGINE1_TEMP_ENG_LIMIT,   // Engine temperature emergency limit
  ENGINE1_TEMP_ROOM,        // Room temperature sensor 1-wire address
  ENGINE1_TEMP_ROOM_LIMIT,  // Room temperature emergency limit
  &sensors
);
#endif

#ifdef ENGINE2_NAME
#define ENGINE2
Engine engine2(
  ENGINE2_NAME,             // Engine name
  ENGINE2_TEMP_ENG,         // Engine temperature sensor 1-wire address
  ENGINE2_TEMP_ENG_LIMIT,   // Engine temperature emergency limit
  ENGINE2_TEMP_ROOM,        // Room temperature sensor 1-wire address
  ENGINE2_TEMP_ROOM_LIMIT,  // Room temperature emergency limit
  &sensors
);
#endif

#ifdef ENGINE1
DigitalInput alarmOilEngine1("Engine 1", PIN_INPUT_RELAY_OIL_ENG_1, "Oil Pressure");
#endif
#ifdef ENGINE2
DigitalInput alarmOilEngine2("Engine 2", PIN_INPUT_RELAY_OIL_ENG_2, "Oil Pressure");
#endif
DigitalInput alarmSmoke("Engine Room", PIN_INPUT_RELAY_SMOKE, "Smoke Alarm");

// Unfortunately the interrupt-related stuff (namely the button) has to be a static
// function. So no class-stuff here!
#include "../lib/button.cpp"

#ifdef OTA
#include "../lib/ota.cpp"
#endif

#ifdef WIFI
void showDeviceStatus() {
  String text = "EngineGuard: " + String(MDNS_NAME) + "\n\n";

#ifdef ENGINE1
  text = text + "Status for engine 1:\n\n" + engine1.getStatus() + "\n\n";
#else
  text = text + "Engine 1 is not defined\n\n";
#endif

#ifdef ENGINE2
  text = text + "Status for engine 2:\n\n" + engine2.getStatus() + "\n\n";
#else
  text = text + "Engine 2 is not defined\n\n";
#endif

  webServer.send(200, "text/plain", text);
}
#endif

void setup(void) {
  pinMode(PIN_ONBOARD_LED, OUTPUT);
  digitalWrite(PIN_ONBOARD_LED, HIGH);

  Serial.begin(115200);

  display.showMessage("Don't panic!");

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

  webServer.on("/", showDeviceStatus);
  webServer.begin();
#endif

#ifdef OTA
  Serial.print("Initialising OTA...");
  initialiseArduinoOTA(MDNS_NAME, OTA_PASSWORD);
  Serial.println("OK");
#endif

  display.clearDisplay();
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
#ifdef ENGINE1
  if (engine1.isEmergency()) {
    return engine1.getEmergencyReason();
  }

  if (alarmOilEngine1.isEmergency()) {
    return alarmOilEngine1.getEmergencyReason();
  }
#endif

#ifdef ENGINE2
  if (engine2.isEmergency()) {
    return engine2.getEmergencyReason();
  }

  if (alarmOilEngine2.isEmergency()) {
    return alarmOilEngine2.getEmergencyReason();
  }
#endif

  if (alarmSmoke.isEmergency()) {
    return alarmSmoke.getEmergencyReason();
  }

  return getZeroEmergencyReason();
}

void loop(void) {
  Serial.println("---");

#ifdef ENGINE1
  engine1.readSensors();
#endif

#ifdef ENGINE2
  engine2.readSensors();
#endif

  const EmergencyReason EmergencyReason = checkEmergency();

#ifndef ENGINE1
  display.updateDisplay(EmergencyReason);
#else
  #ifndef ENGINE2
  display.updateDisplay(&engine1, EmergencyReason);
  #else
  display.updateDisplay(&engine1, &engine2, EmergencyReason);
  #endif
#endif

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

  webServer.handleClient();
#endif

#ifdef OTA
  ArduinoOTA.handle();
#endif
}
