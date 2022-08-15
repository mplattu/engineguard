#ifndef ENGINEGUARD_ENGINE_H
#define ENGINEGUARD_ENGINE_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class Engine {
private:
  String engineName;
  String onewireAddressEngine;
  String onewireAddressRoom;
  float lastTemperatureEngine;
  float lastTemperatureRoom;
  DallasTemperature * onewireSensors;
  String oneWireDeviceAddressToString(DeviceAddress onewireAddress);
public:
  Engine(String engineName, String onewireAddressEngine, String onewireAddressRoom, DallasTemperature * sensors);
  String getName();
  char* getNameCh();
  void readSensors();
  float getTemperatureEngine();
  char* getTemperatureEngineCh();
  float getTemperatureRoom();
  char* getTemperatureRoomCh();
};

#endif
