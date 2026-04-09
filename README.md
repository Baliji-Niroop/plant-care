# ESP32 Smart Plant Irrigation

An ESP32 irrigation controller with:

- Modular production firmware in `firmware/`
- A local Wokwi digital twin in `simulation/wokwi/`
- A repeatable validation checklist in `docs/`

## What this project guarantees

The controller waters only when all safety conditions pass:

- Tank interlock: blocks watering when the tank is empty.
- Cooldown control: prevents rapid on/off pump cycling.
- Watchdog timeout: stops pump runtime that exceeds the safety limit.
- Sensor fail-safe: invalid sensor frames default to HOLD.

## Quick start (beginner-first)

1. Install dependencies:
   - `arduino-cli core update-index`
   - `arduino-cli core install esp32:esp32@3.3.7`
   - `arduino-cli lib install "DHT sensor library for ESPx"`
   - `arduino-cli lib install "DHTesp"`
2. In VS Code Tasks, run: `Wokwi: Build Firmware`
3. Then run: `Wokwi: Start Simulator`
4. Confirm artifacts exist in `simulation/wokwi/build/`:
   - `sketch.ino.bin`
   - `sketch.ino.elf`

## Validation flow

Use `docs/simulation-validation-checklist.md` and verify all scenarios:

- Wet-soil hold
- Dry-soil watering
- Tank-empty interlock
- Cooldown behavior

If all checks pass, the digital twin is considered healthy.

## Key configuration

Primary tuning values are in `firmware/include/config.h`:

- `SOIL_DRY_THRESHOLD_PERCENT`
- `DECISION_DEBOUNCE_READINGS`
- `PUMP_ON_DURATION_MS`
- `PUMP_COOLDOWN_MS`
- `PUMP_WATCHDOG_MS`

## Repo map

| Area | Purpose |
|---|---|
| `firmware/` | Production firmware and reusable control modules |
| `simulation/wokwi/` | Local digital twin, build scripts, Wokwi assets |
| `docs/` | Validation notes and evidence checklist |
| `hardware/` | Hardware references and assembly guidance |

## Troubleshooting

- If simulator does not open from task, use `F1 -> Wokwi: Start Simulator`.
- If build fails, rerun dependency install commands and retry build.
- ESP32 BT pragma lines during compile are informational notes, not failures.

Project by Niroop Baliji.
