# ESP32 Smart Plant Irrigation

This project is an automatic plant watering system built with ESP32.
It includes:

- production firmware in `firmware/`
- a Wokwi simulator in `simulation/wokwi/`
- validation notes in `docs/`
- hardware build docs in `hardware/`

## How it works

The pump turns on only when all conditions are safe:

1. Soil is dry enough.
2. Tank has water.
3. Cooldown time is over.
4. Pump runtime is still inside watchdog limit.

If sensor data is invalid, firmware chooses **HOLD** (safe behavior).

## Quick simulation setup

1. Install tools and libraries:
   - `arduino-cli core update-index`
   - `arduino-cli core install esp32:esp32@3.3.7`
   - `arduino-cli lib install "DHT sensor library for ESPx"`
   - `arduino-cli lib install "DHTesp"`
2. Open `simulation/wokwi/wokwi.toml` and set:
   - `wokwi_license = "YOUR_WOKWI_LICENSE_KEY"`
3. Run VS Code task: **Wokwi: Build Firmware**
4. Run VS Code task: **Wokwi: Start Simulator**
5. Check output files:
   - `simulation/wokwi/build/sketch.ino.bin`
   - `simulation/wokwi/build/sketch.ino.elf`

## Very important before hardware flashing

- Flash only: `firmware/firmware.ino`
- Do **not** flash: `simulation/wokwi/sketch.ino` (simulation-only)

Before flashing, confirm `firmware/include/config.h`:

- relay polarity:
  - `RELAY_ON`
  - `RELAY_OFF`
- tank switch polarity:
  - `TANK_SWITCH_PIN_MODE`
  - `TANK_WATER_PRESENT_LEVEL`

Default tank wiring is `INPUT_PULLUP` with switch between GPIO 5 and GND, so water-present level is `LOW`.

## Main tuning values

All in `firmware/include/config.h`:

- `SOIL_DRY_THRESHOLD_PERCENT`
- `DECISION_DEBOUNCE_READINGS`
- `PUMP_ON_DURATION_MS`
- `PUMP_COOLDOWN_MS`
- `PUMP_WATCHDOG_MS`

## Validation checklist

Use `docs/simulation-validation-checklist.md` to verify:

- wet-soil hold
- dry-soil watering
- tank-empty interlock
- cooldown behavior

## Project structure

| Path | Purpose |
|---|---|
| `firmware/` | Real hardware firmware |
| `simulation/wokwi/` | Local simulator and scripts |
| `docs/` | Test checklist and notes |
| `hardware/` | Wiring and assembly documentation |
