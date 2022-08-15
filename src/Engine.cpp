#include "Engine.h"

Engine::Engine(String engineName, String onewireAddressEngine, String onewireAddressRoom, DallasTemperature * sensors) {
  this->engineName = engineName;
  this->onewireAddressEngine = onewireAddressEngine;
  this->onewireAddressRoom = onewireAddressRoom;
  this->lastTemperatureEngine = 0.0;
  this->lastTemperatureRoom = 0.0;

  this->onewireSensors = sensors;
}

String Engine::getName() {
  return this->engineName;
}

char* Engine::getNameCh() {
  char* name;
  name = &this->engineName[0];
  return name;
}

void Engine::readSensors() {
  int numberOfSensors = this->onewireSensors->getDeviceCount();

  this->onewireSensors->requestTemperatures();

  DeviceAddress currentDeviceAddress;
  String currentDeviceAddressStr;

  for (int i=0; i < numberOfSensors; i++) {
    if (this->onewireSensors->getAddress(currentDeviceAddress, i)) {
      currentDeviceAddressStr = this->oneWireDeviceAddressToString(currentDeviceAddress);

      Serial.print(currentDeviceAddressStr + " ");

      float temperature = this->onewireSensors->getTempC(currentDeviceAddress);
      Serial.print(temperature);

      if (currentDeviceAddressStr == this->onewireAddressEngine) {
        Serial.print(" belongs to " + this->engineName + " (engine)");
        this->lastTemperatureEngine = temperature;
      }

      if (currentDeviceAddressStr == this->onewireAddressRoom) {
        Serial.print(" belongs to " + this->engineName + " (room)");
        this->lastTemperatureRoom = temperature;
      }

      Serial.println();
    }
  }
}

float Engine::getTemperatureEngine() {
  return this->lastTemperatureEngine;
}

char* Engine::getTemperatureEngineCh() {
  float temperature = this->getTemperatureEngine();

  char* buffer = new char[5];
  dtostrf(temperature, 4, 0, buffer);
  return buffer;
}

float Engine::getTemperatureRoom() {
  return this->lastTemperatureRoom;
}

char* Engine::getTemperatureRoomCh() {
  float temperature = this->getTemperatureRoom();

  char* buffer = new char[5];
  dtostrf(temperature, 4, 0, buffer);
  return buffer;
}

String Engine::oneWireDeviceAddressToString(DeviceAddress onewireAddress) {
  String onewireAddressStr = "";

  for (uint8_t i = 0; i < 8; i++){
    if (onewireAddress[i] < 16) {
      onewireAddressStr += "0";
    }
    onewireAddressStr += String(onewireAddress[i], HEX);
  }

  return onewireAddressStr;
}
