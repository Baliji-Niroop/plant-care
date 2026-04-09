# ESP32 Smart Plant Irrigation

An ESP32 irrigation controller with a modular firmware path for the real device and a local Wokwi digital twin for repeatable validation.

## Overview

The controller reads soil moisture, temperature, humidity, and tank level before deciding whether watering is safe. The software includes three core safeguards:

- **Tank interlock** prevents dry-run operation.
- **Cooldown control** prevents rapid pump cycling.
- **Watchdog timeout** stops the pump if runtime exceeds the limit.

## Repository layout

| Area | Purpose |
|---|---|
| `firmware/` | Production firmware and reusable control modules |
| `simulation/wokwi/` | Local digital twin, build scripts, and Wokwi assets |
| `docs/` | Validation notes and scenario coverage |
| `hardware/` | Hardware references and later-stage prototype work |

## Local workflow

1. Install dependencies:
   - `arduino-cli core update-index`
   - `arduino-cli core install esp32:esp32@3.3.7`
   - `arduino-cli lib install "DHT sensor library for ESPx"`
   - `arduino-cli lib install "DHTesp"`
2. Run **Wokwi: Build Firmware** from the VS Code task list.
3. Run **Wokwi: Start Simulator**.
4. Review the generated firmware artifacts in `simulation/wokwi/build/`.

## Configuration

Primary tuning values live in `firmware/include/config.h`:

- `SOIL_DRY_THRESHOLD_PERCENT`
- `PUMP_ON_DURATION_MS`
- `PUMP_COOLDOWN_MS`
- `PUMP_WATCHDOG_MS`

## Documentation

| Document | Purpose |
|---|---|
| `simulation/wokwi/README.md` | Simulator setup and usage |
| `docs/simulation-validation-checklist.md` | Scenario list and evidence notes |
| `hardware/BUILD_GUIDE.md` | Hardware work for later stages |

## Status

The software side is structured for review and simulation. Hardware work remains deferred.

Project by Niroop Baliji.
