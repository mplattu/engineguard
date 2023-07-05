#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EspSigK.h>

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
#define PIN_INPUT_RELAY_GAS 26

#include "../include/settings.cpp"

// Network
#ifdef WIFI_SSID
#define WIFI
#include <WiFi.h>
#include <WebServer.h>
WebServer webServer(80);
WiFiClient wiFiClient;
EspSigK sigK(MDNS_NAME, WIFI_SSID, WIFI_PASS, &wiFiClient);
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
DigitalInput alarmGas("Kitchen", PIN_INPUT_RELAY_GAS, "Gas Alarm");

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

bool wifiConnected() {
#ifdef WIFI
  return (WiFi.status() == WL_CONNECTED);
#else
  return false;
#endif  
}

void setup(void) {
  pinMode(PIN_ONBOARD_LED, OUTPUT);
  digitalWrite(PIN_ONBOARD_LED, HIGH);

  Serial.begin(115200);

  display.showMessage("Init: Sensors");

  sensors.begin();

  display.showMessage("Init: Button");
  buttonSetup(PIN_CLEAR_BUTTON, CLEAR_BUTTON_VALIDITY_IN_SECONDS);

#ifdef WIFI
  sigK.setServerHost(SIGNALK_SERVER_IP);
  sigK.setServerPort(SIGNALK_SERVER_PORT);
  sigK.setPrintDebugSerial(true);

  display.showMessage("Init: SignalK");
  sigK.begin();

  display.showMessage("Init: Webserver"); 
  webServer.on("/", showDeviceStatus);
  webServer.begin();
#endif

#ifdef OTA
  display.showMessage("Init: OTA");
  Serial.print("Initialising OTA...");
  initialiseArduinoOTA((char*)MDNS_NAME, (char*)OTA_PASSWORD);
  Serial.println("OK");
#endif

#ifdef ENGINE1
  #ifdef ENGINE2
  display.showEmergencyLimits(&engine1, &engine2);
  #else
  display.showEmergencyLimits(&engine1);
  #endif
  delay(3000);
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

  if (alarmGas.isEmergency()) {
    return alarmGas.getEmergencyReason();
  }

  return getZeroEmergencyReason();
}

void safeDelay(unsigned long ms) {
#ifdef WIFI
  sigK.safeDelay(ms);
#else
  delay(ms);
#endif
}

int getCelsiusInKelvin(float tempCelsius) {
  return (int)(tempCelsius + 273.15);
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
  display.updateDisplay(EmergencyReason, wifiConnected());
#else
  #ifndef ENGINE2
  display.updateDisplay(&engine1, EmergencyReason, wifiConnected());
  #else
  display.updateDisplay(&engine1, &engine2, EmergencyReason, wifiConnected());
  #endif
#endif

  sound.updateSignal(EmergencyReason, buttonPressIsStillValid());

  buttonLoop();

  digitalWrite(PIN_ONBOARD_LED, LOW);
  safeDelay(250);
  digitalWrite(PIN_ONBOARD_LED, HIGH);
  safeDelay(250);

#ifdef WIFI
#ifdef ENGINE1
  sigK.addDeltaValue("propulsion.port.coolantTemperature", getCelsiusInKelvin(engine1.getTemperatureEngine()));
  sigK.addDeltaValue("environment.inside.engineroomPort.temperature", getCelsiusInKelvin(engine1.getTemperatureRoom()));
#endif
#ifdef ENGINE2
  sigK.addDeltaValue("propulsion.starboard.coolantTemperature", getCelsiusInKelvin(engine2.getTemperatureEngine()));
  sigK.addDeltaValue("environment.inside.engineroomStarboard.temperature", getCelsiusInKelvin(engine2.getTemperatureRoom()));
#endif
  sigK.sendDelta();
  webServer.handleClient();
#endif

#ifdef OTA
  ArduinoOTA.handle();
#endif
}
