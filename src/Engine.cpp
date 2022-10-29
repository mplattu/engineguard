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

  this->lastReadStatus = "";
}

String Engine::getName() {
  return this->engineName;
}

String Engine::getStatus() {
  String statusText = "";
  statusText += "Engine name: " + this->getName() + "\n";
  statusText += "Engine sensor address: " + this->onewireAddressEngine + "\n";
  statusText += "Room sensor address: " + this->onewireAddressRoom + "\n";

  statusText += this->lastReadStatus;

  return statusText;
}

void Engine::readSensors() {
  String newStatus = "";
  int numberOfSensors = this->onewireSensors->getDeviceCount();

  this->onewireSensors->requestTemperatures();

  DeviceAddress currentDeviceAddress;
  String currentDeviceAddressStr;

  if (numberOfSensors == 0) {
    newStatus += "No devices found\n";
  }

  for (int i=0; i < numberOfSensors; i++) {
    if (this->onewireSensors->getAddress(currentDeviceAddress, i)) {
      currentDeviceAddressStr = this->oneWireDeviceAddressToString(currentDeviceAddress);

      Serial.print(currentDeviceAddressStr + " ");
      newStatus += String(i) + ": " + currentDeviceAddressStr;

      float temperature = this->onewireSensors->getTempC(currentDeviceAddress);
      Serial.print(temperature);
      newStatus += " temp: " + String(temperature);

      if (currentDeviceAddressStr == this->onewireAddressEngine) {
        Serial.print(" belongs to " + this->engineName + " (engine)");
        newStatus += " (engine)";
        this->lastTemperatureEngine = temperature;
      }

      if (currentDeviceAddressStr == this->onewireAddressRoom) {
        Serial.print(" belongs to " + this->engineName + " (room)");
        newStatus += " (room)";
        this->lastTemperatureRoom = temperature;
      }

      Serial.println();
      newStatus += "\n";
    }
  }

  this->lastReadStatus = newStatus;
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

bool Engine::isEmergency() {
  if (this->lastTemperatureEngine > this->emergencyLimitEngine) {
    this->lastEmergencyReason.isEmergency = true;
    this->lastEmergencyReason.target = this->engineName;
    this->lastEmergencyReason.reason = "Engine temp";
    this->lastEmergencyReason.value = this->getTemperatureEngineStr();
    this->lastEmergencyReason.hasValue = true;

    return true;
  }

  if (this->lastTemperatureRoom > this->emergencyLimitRoom) {
    this->lastEmergencyReason.isEmergency = true;
    this->lastEmergencyReason.target = this->engineName;
    this->lastEmergencyReason.reason = "Eng room temp";
    this->lastEmergencyReason.value = this->getTemperatureRoomStr();
    this->lastEmergencyReason.hasValue = true;

    return true;
  }

  this->lastEmergencyReason = getZeroEmergencyReason();
  
  return false;
}

EmergencyReason Engine::getEmergencyReason() {
  return this->lastEmergencyReason;
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
