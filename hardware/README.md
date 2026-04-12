# Hardware Design Reference

This document explains the real hardware setup used by the project.

## 1. Hardware objective

Use ESP32 to read plant and tank conditions, then drive a relay-controlled pump only when watering is required and safe.

## 2. Inputs and output

Inputs:

- capacitive soil moisture sensor (analog)
- DHT22 temperature and humidity sensor
- float switch for tank water availability

Output:

- relay control signal for 5V pump switching

## 3. Required hardware components

| Component | Role |
|---|---|
| ESP32 DevKit V1 | Main controller |
| Capacitive soil moisture sensor | Soil moisture input |
| DHT22 | Temperature and humidity input |
| Float switch (NO) | Tank availability interlock |
| 1-channel relay module | Pump switching element |
| 5V DC mini pump | Water delivery actuator |

Full sourcing and cost notes are listed in `components/bom.txt`.

## 4. ESP32 pin map (production firmware)

| Signal | ESP32 pin |
|---|---|
| Soil analog output | GPIO34 |
| DHT22 data | GPIO4 |
| Float switch input | GPIO5 |
| Relay input | GPIO18 |

## 5. Power design

- 3.3V rail: soil sensor and DHT22 logic
- external 5V rail: relay module and pump path
- shared ground between ESP32 and external supply is mandatory

Reason: pump inrush on a weak shared source can reset ESP32.

## 6. Polarity and electrical settings

### Relay polarity

Relay modules can be active-high or active-low. Confirm and set in `firmware/include/config.h`:

- `RELAY_ON`
- `RELAY_OFF`

### Tank switch polarity

Current default configuration:

- `TANK_SWITCH_PIN_MODE = INPUT_PULLUP`
- switch wired between GPIO5 and GND
- `TANK_WATER_PRESENT_LEVEL = LOW`

With this default, LOW means water is present.

## 7. Calibration references used by production firmware

Soil calibration references:

- dry reference: 3950
- wet reference: 1650

These are used to map raw ADC readings to moisture percentage.

## 8. Safety behavior implemented in hardware workflow

- invalid sensor frame results in HOLD
- tank-empty blocks watering
- cooldown blocks immediate retrigger
- watchdog forces pump stop on excessive runtime

## 9. Wiring source

Detailed wiring graph:

- `wiring/wiring-diagram.mmd`

## 10. Hardware firmware target

For physical ESP32 deployment, flash only:

- `firmware/firmware.ino`

Do not flash:

- `simulation/wokwi/sketch.ino`
