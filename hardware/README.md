# Hardware README

This file explains the hardware in simple terms: parts, wiring logic, safety, and practical notes.

## 1) System summary

The controller uses ESP32 to read sensors and control a pump through a relay.

Main inputs:
- Soil moisture sensor (analog)
- DHT22 (temperature + humidity)
- Float switch (tank level)

Main output:
- Relay signal to switch the 5V pump

## 2) Core hardware parts

| Part | Use | Notes |
|---|---|---|
| ESP32 DevKit V1 | Main controller | Fast enough for 2s loop and telemetry |
| Capacitive soil sensor | Moisture level input | Better long-term reliability than resistive type |
| DHT22 | Temp/humidity input | Stable and common in Arduino projects |
| Float switch (NO) | Tank water availability | Safety interlock input |
| 1-channel relay module | Pump switching | Check relay polarity before flashing |
| 5V DC mini pump | Water delivery | Use separate 5V supply |

## 3) Pin mapping

| Signal | ESP32 pin |
|---|---|
| Soil analog output | GPIO 34 |
| DHT22 data | GPIO 4 |
| Float switch input | GPIO 5 |
| Relay control input | GPIO 18 |

Power:
- 3.3V rail: soil sensor + DHT22 logic
- External 5V rail: relay + pump
- Common GND between ESP32 and external 5V supply is mandatory

## 4) Important polarity settings

### Relay polarity

Relay modules are not all the same:
- some are active-high (`HIGH = ON`)
- some are active-low (`LOW = ON`)

Set these in `firmware/include/config.h`:
- `RELAY_ON`
- `RELAY_OFF`

### Tank switch polarity (already configurable)

Production firmware uses config values:
- `TANK_SWITCH_PIN_MODE`
- `TANK_WATER_PRESENT_LEVEL`

Default:
- `INPUT_PULLUP`
- switch wired between GPIO 5 and GND
- water-present level is `LOW`

## 5) Soil sensor calibration used by firmware

Production firmware calibration (current values):
- dry reference: `3950`
- wet reference: `1650`

This is used to convert ADC to moisture percent.
Recalibrate if your soil/sensor behaves differently.

## 6) Safety behavior in hardware

The firmware applies three safety layers:

1. Tank interlock: no watering if tank is empty
2. Cooldown delay: prevents rapid repeated pumping
3. Watchdog timeout: force stops pump if runtime exceeds limit

This protects pump life and reduces risky behavior.

## 7) Power guidance

Use separate power paths:
- ESP32 from USB
- pump/relay from external 5V source (1A+ recommended)

Reason:
- pump startup current can reset ESP32 if everything is on one weak USB source

## 8) Wiring diagram file

Mermaid source file:
- `hardware/wiring-diagram.mmd`

It shows full signal and power routing, including the default tank switch and relay polarity notes.

## 9) Bill of materials (quick)

| Component | Approx cost (INR) |
|---|---|
| ESP32 DevKit V1 | 550 |
| Capacitive soil sensor | 200 |
| DHT22 module | 150 |
| 1-channel relay | 80 |
| 5V mini pump | 300 |
| Float switch | 100 |
| Breadboard | 200 |
| Jumper wires | 80 |
| USB cable / PSU | 150 |
| Tubing + misc | 150 |
| **Total (approx)** | **1960** |

## 10) Final note

For real hardware, always use:
- `firmware/firmware.ino`

Do not use the Wokwi simulation sketch for hardware flashing.
