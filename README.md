# EngineGuard

Yacht engine room monitor based on ESP32 monitoring following sensors:
 * Temperature of engines 1 & 2 (DS18B20 temperature sensor)
 * Temperature of engine room above engines 1 & 2 (DS18B20 temperature sensor)
 * Oil pressure from engines 1 & 2 (+24V DC from VDO oil pressure sensor)
 * Engine room smoke sensor

Intended operation:
 1. Any of the sensors report an emergency.
 1. An internal buzzers gives signal. This is mainly for debugging the device.
 1. An external 24V device gives signal. The device is connected via a reed relay.
 1. The OLED shows the reason for the alarm.
 1. The external signal can be cut for certain period of time by pressing a button.

## Project Status

 1. We have a working code for all the sensors.
 1. We are testing the PCB v3.

## Technical Details

### Schematic and PCBs

See `board/` for Fritzing schematic & PCB.

### Components

* U1 ESP-WROOM-32
* U2 5V reed relay with protective diode, Meder SIL05-1A72-71D ([Local electronics shop](https://www.partco.fi/fi/saehkoemekaniikka/releet/reed-releet/7411-rr-05-sild.html))
* U3-U6 24V reed relay with protective diode, Meder SIL24-1A72-71D ([Local electronics shop](https://www.partco.fi/fi/saehkoemekaniikka/releet/reed-releet/7413-rr-24-sild.html))
* OLED screen with SSD1309 driver, I2C version ([Aliexpress](https://www.aliexpress.com/item/1005003787866468.html))
* C1, C2 unidirectional capacitor 0,47 mF
* D1 schottky diode 1N5817
* LED1 5mm LED in THT package
* R1 resistor 330 ohm
* R2 resistor 4,7 kOhm
* Box for central unit including J1 and J2 ([Aliexpress](https://www.aliexpress.com/item/1005003153480194.html))
* J3 internal piezo 3V ([Local electronics shop](https://www.partco.fi/fi/audiovideo/summerit/20187-summeri-3v.html))
* Box for screen and cancel button ([Aliexpress](https://www.aliexpress.com/item/4000081121421.html))
* Cancel button
* Smoke Detector: Orbis Marine Multisensor Detector [ORB-OH-43003-MAR](https://www.apollo-fire.co.uk/products/orbis-marine/smokeheat-detectors/orb-oh-43003-mar---orbis-marine-multi-sensor-detector-opticalheat---flashing-led/) and Orbis Marine Relay Base [ORB-RB-40004-MAR](https://www.apollo-fire.co.uk/products/orbis-marine/relay-bases/orb-rb-40004-mar---orbis-marine-timesaver-base---relay/)

When selecting OLED screen make sure its driver chip is supported by
[u8g2 library](https://github.com/olikraus/u8g2/wiki).

### Connectors

Input

  1. DC VCC (9-32V)
  1. DC GND
  1. Engine room smoke detector (+24V) - GPIO 33
  1. Engine room smoke detector (GND)
  1. Engine 1 Oil (+24V) - GPIO 25
  1. Engine 1 Oil (GND)
  1. Engine 2 Oil (+24V) - GPIO 32
  1. Engine 2 Oil (GND)
  1. Spare input (+24V) - GPIO 26
  1. Spare input (GND)
  1. Engine room temperature 1-wire, VCC (red)
  1. Engine room temperature 1-wire, GND (black)
  1. Engine room temperature 1-wire, signal (yellow) - GPIO 4
  1. unused

Output

  1. DC +5V (OLED)
  1. DC GND (OLED)
  1. I2C SDA (OLED) - GPIO 21
  1. I2C SCL (OLED) - GPIO 22
  1. Alarm bell (DC VCC 9-32V from input pin 1) - GPIO 19 via relay U2
  1. Alarm bell GND
  1. Clear button A (closed when pressed) - GPIO 23
  1. Clear button A (closed when pressed) - GND
  1. unused
  1. unused
  1. unused
  1. unused
  1. unused
  1. unused
  * Internal buzzer - GPIO 18

## Software

### Building and Uploading the Code

 * Install PlatformIO
 * Copy the configuration template `include/settings.cpp.sample` to `include/settings.cpp`.
   Make necessary changes.
 * If you are planning to use OTA updates copy `include/settings.mk.sample` to `include/settings.mk`.
   Make necessary changes.
 * You'll find more configurations in `src/engineguard.cpp`. If you don't
   have twin engines just remove references to the engine 2.
 * `make upload` should build and upload the code to your ESP32 via USB.
 * `make upload_ota` should do the same as OTA update after you have uploaded the first
   version of the code via USB.
 * I had problems uploading the code to an unused ESP32. It appears that my chip was
   missing some boot code. Uploading a simple Hello World from Arduino IDE did the
   trick.

### Identifying OneWire Temperature Sensors

The OneWire sensor addresses are hard-coded in `engineguard.cpp`. Within each
cycle EngineGuard enumerates all OneWire devices (`make monitor`). This is an easy
way to find out which sensors are connected and which devices are already claimed
by a known sensor.

Sample output:

```
28309d1200000097 29.37 belongs to Eng1 (engine)
289cef1400000051 29.37
28decb14000000ad 27.12 belongs to Eng1 (room)
28ddd61400000064 27.06
28309d1200000097 29.37
289cef1400000051 29.37 belongs to Eng2 (engine)
28decb14000000ad 27.12
28ddd61400000064 27.06 belongs to Eng2 (room)
```
