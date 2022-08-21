#include "Engine.h"

Engine::Engine(String engineName, String onewireAddressEngine, float emergencyLimitEngine, String onewireAddressRoom, float emergencyLimitRoom, DallasTemperature * sensors) {
  this->engineName = engineName;
  this->onewireAddressEngine = onewireAddressEngine;
  this->onewireAddressRoom = onewireAddressRoom;
  this->emergencyLimitEngine = emergencyLimitEngine;
  this->emergencyLimitRoom = emergencyLimitRoom;

  this->lastTemperatureEngine = 0.0;
  this->lastTemperatureRoom = 0.0;

  this->onewireSensors = sensors;
}

String Engine::getName() {
  return this->engineName;
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

String Engine::getTemperatureEngineStr() {
  float temperature = this->getTemperatureEngine();

  return String(temperature, 1) + "°C";
}

float Engine::getTemperatureRoom() {
  return this->lastTemperatureRoom;
}

String Engine::getTemperatureRoomStr() {
  float temperature = this->getTemperatureRoom();

  return String(temperature, 1) + "°C";
}

bool Engine::isEmergencyEngine() {
  if (this->lastTemperatureEngine > this->emergencyLimitEngine) {
    return true;
  }

  return false;
}

bool Engine::isEmergencyRoom() {
  if (this->lastTemperatureRoom > this->emergencyLimitRoom) {
    return true;
  }

  return false;
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
