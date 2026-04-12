# Hardware Build Guide (Bench Prototype)

This guide describes a clean assembly and validation sequence for the current hardware architecture.

## 1. Pre-assembly checklist

- [ ] all parts available (`../components/bom.txt`)
- [ ] ESP32 ready for production firmware (`firmware/firmware.ino`)
- [ ] USB cable and serial monitor access
- [ ] breadboard and jumper wires
- [ ] external 5V supply for relay and pump

## 2. Recommended wiring sequence

### Step A: establish shared ground and power rails

Connect:

- external 5V+ to breadboard positive rail
- external GND to breadboard ground rail
- ESP32 GND to same ground rail

Common ground is mandatory for stable control signaling.

### Step B: wire soil moisture sensor

| Sensor pin | Connection | Notes |
|---|---|---|
| VCC | ESP32 3.3V | ADC-safe logic level |
| GND | Common GND | shared reference |
| AO | ESP32 GPIO34 | analog input |
| DO | not used | leave unconnected |

Quick check reference:

- dry-air reading is typically near 3950
- wet reference reading is typically near 1650

### Step C: wire DHT22

| DHT22 pin | Connection | Notes |
|---|---|---|
| VCC | ESP32 3.3V | do not use 5V here |
| GND | Common GND | shared reference |
| DATA | ESP32 GPIO4 | digital signal |
| NC | not used | leave unconnected |

Quick check:

- serial output should show temperature and humidity
- first startup frame can be transiently invalid

### Step D: wire float switch (tank level)

| Float switch terminal | Connection | Notes |
|---|---|---|
| A | ESP32 GPIO5 | input pin |
| B | GND | for pull-up default wiring |

Default firmware configuration:

- `INPUT_PULLUP`
- water present when GPIO5 reads `LOW`

### Step E: wire relay module

| Relay pin | Connection | Notes |
|---|---|---|
| VCC | external 5V | relay power |
| GND | common GND | shared reference |
| IN | ESP32 GPIO18 | control signal |
| COM | external 5V+ | switched source |
| NO | pump positive | switched output |

Before operation, confirm relay polarity settings in `firmware/include/config.h`.

### Step F: wire pump

| Pump lead | Connection |
|---|---|
| Pump + | relay NO |
| Pump - | external supply GND |

Safety expectation:

- pump is OFF at boot
- pump runs only when relay command is active

## 3. Wiring cross-check

Use `wiring-diagram.mmd` as final reference before powering the system.

## 4. Bench validation workflow

Open serial monitor at 115200 baud.

Expected startup lines:

- `ESP32 Smart Plant Irrigation`
- `System ready. Monitoring started.`

Run these tests:

1. dry-soil trigger: pump should start after debounce criteria
2. wet-soil hold: pump should remain off
3. tank-empty interlock: dry soil must not start pump when tank is empty
4. cooldown behavior: immediate retrigger must be blocked

## 5. Troubleshooting reference

| Symptom | Likely cause | Recommended action |
|---|---|---|
| No serial output | wrong COM port/baud/driver | verify selected port and 115200 baud |
| Soil fixed at 0 or 4095 | analog wiring error | re-check GPIO34 and sensor AO wiring |
| Repeated DHT invalid frames | power/noise/wiring issue | verify 3.3V, GND, and data routing |
| Tank state does not change | switch wiring mismatch | verify GPIO5 path and switch orientation |
| Relay acts inverted | relay polarity mismatch | adjust `RELAY_ON` and `RELAY_OFF` |
| Pump does not run | switched power path issue | verify COM/NO and 5V supply path |
| ESP32 resets at pump start | supply voltage sag | strengthen external 5V supply |

## 6. Validation evidence

Capture:

1. top-view wiring photo
2. photo during pump active phase
3. serial snippets for key scenarios

## 7. Post-bench tasks

- [ ] re-check calibration references in firmware
- [ ] run a longer-duration stability test
- [ ] prepare migration to enclosure or proto-board
