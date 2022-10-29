#ifndef ENGINEGUARD_ENGINE_H
#define ENGINEGUARD_ENGINE_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "engineguard.h"

class Engine {
private:
  String engineName;
  String onewireAddressEngine;
  String onewireAddressRoom;
  float lastTemperatureEngine;
  float lastTemperatureRoom;
  float emergencyLimitEngine;
  float emergencyLimitRoom;
  String lastReadStatus;
  EmergencyReason lastEmergencyReason;
  DallasTemperature * onewireSensors;
  String oneWireDeviceAddressToString(DeviceAddress onewireAddress);
public:
  Engine(String engineName, String onewireAddressEngine, float emergencyLimitEngine, String onewireAddressRoom, float emergencyLimitRoom, DallasTemperature * sensors);
  String getName();
  String getStatus();
  void readSensors();
  float getTemperatureEngine();
  String getTemperatureEngineStr();
  float getTemperatureRoom();
  String getTemperatureRoomStr();
  bool isEmergency();
  EmergencyReason getEmergencyReason();
};

#endif
