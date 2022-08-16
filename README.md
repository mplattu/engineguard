# EngineGuard

Yacht engine room monitor based on ESP32

## Identifying OneWire Temperature Sensors

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
