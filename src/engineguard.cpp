#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Engine.h"

#define PIN_DISPLAY_CLOCK 22
#define PIN_DISPLAY_DATA 21
#define PIN_ONEWIRE 4

// Display
U8X8_SSD1309_128X64_NONAME2_SW_I2C u8x8(PIN_DISPLAY_CLOCK, PIN_DISPLAY_DATA);

// Onewire and sensors
OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);

// We have two engines
Engine engine1("Eng1", "28309d1200000097", "28decb14000000ad", &sensors);
Engine engine2("Eng2", "289cef1400000051", "28ddd61400000064", &sensors);

void setup(void) {
  Serial.begin(115200);
  u8x8.begin();
  u8x8.setFont(u8x8_font_7x14B_1x2_f);

  sensors.begin();
}

void updateDisplay(void) {
  u8x8.drawUTF8(0, 0, engine1.getNameCh());
  u8x8.drawUTF8(0, 2, engine2.getNameCh());

  u8x8.drawUTF8(5, 0, engine1.getTemperatureEngineCh());
  u8x8.drawUTF8(5, 2, engine2.getTemperatureEngineCh());

  u8x8.drawUTF8(10, 0, engine1.getTemperatureRoomCh());
  u8x8.drawUTF8(10, 2, engine2.getTemperatureRoomCh());
}

void loop(void) {
  Serial.println("---");

  engine1.readSensors();
  engine2.readSensors();

  updateDisplay();

  delay(1000);
}
