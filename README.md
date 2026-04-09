# ESP32 Smart Plant Irrigation

An ESP32 irrigation controller with two tracks: modular firmware for real hardware and a local Wokwi digital twin for repeatable validation and demos.

## Project Overview

The controller reads soil moisture, temperature/humidity (DHT22), and tank level, then decides whether watering is allowed. It includes three practical safety controls:
- **Tank interlock**: prevents dry-run by blocking pump when water is unavailable
- **Cooldown delay**: anti-chatter mechanism that enforces pause between watering cycles
- **Watchdog timeout**: hard safety cutoff if pump runtime exceeds expected limit

## Architecture

| Layer | Files |
|---|---|
| Firmware (modular) | `firmware/main.ino`, `firmware/include/config.h`, `firmware/include/sensors.h`, `firmware/include/irrigation.h`, `firmware/include/telemetry.h` |
| Digital twin (Wokwi) | `simulation/wokwi/sketch.ino`, `simulation/wokwi/diagram.json`, `simulation/wokwi/wokwi.toml`, `simulation/wokwi/scripts/` |
| Validation assets | `docs/simulation-validation-checklist.md` |
| Hardware notes | `hardware/components_list.txt`, `hardware/assembly_notes.txt` |

## Local Digital Twin Workflow

1. Install dependencies once:
   - `arduino-cli core update-index`
   - `arduino-cli core install esp32:esp32`
   - `arduino-cli lib install "DHT sensor library for ESPx"`
2. Run **`Wokwi: Build Firmware`** from VS Code tasks.
3. Start **`Wokwi: Start Simulator`**.
4. Verify generated outputs:
   - `simulation/wokwi/build/sketch.ino.bin`
   - `simulation/wokwi/build/sketch.ino.elf`

## Validation Evidence

Validation is documented through serial telemetry logs and scenario outcomes in `docs/simulation-validation-checklist.md`.

## Hardware Roadmap

Current state: digital twin validated firmware logic.

**Planned path:**
1. **Phase 1 - Digital Twin** ✅ Complete: decision logic and safety behavior
2. **Phase 2 - Physical Prototype** (next): bench wiring, pump/tank tests, enclosure
3. **Phase 3 - Cloud Telemetry** (future): remote dashboard and long-run trend logging

### Prototype Stages

**Stage 1 - Bench Prototype** (component integration and initial testing)

📋 **To Update**: Follow `hardware/BUILD_GUIDE.md` to wire components on breadboard and record Stage 1 validation notes.

**Stage 2 - Assembled Prototype** (enclosure and field-ready configuration)

📋 **To Update**: After Stage 1 validation, mount components to proto-board or PCB in enclosure and record final assembly notes.

## Configuration

Main tuning values are in `firmware/include/config.h`:
- `SOIL_DRY_THRESHOLD_PERCENT` (default: 60%)
- `PUMP_ON_DURATION_MS` (default: 5000 ms)
- `PUMP_COOLDOWN_MS` (default: 60000 ms)
- `PUMP_WATCHDOG_MS` (default: 10000 ms)

---

## Documentation

### Quick Start
- **New to project?** Start with this README
- **Building hardware?** See `hardware/BUILD_GUIDE.md`

### Complete Documentation
| Document | Purpose |
|---|---|
| `README.md` (this file) | Project overview and architecture |
| `simulation/wokwi/README.md` | Wokwi simulator setup and usage |
| `docs/simulation-validation-checklist.md` | Complete validation test cases |
| `hardware/README.md` | Hardware build status and calibration |
| `hardware/BUILD_GUIDE.md` | Step-by-step Stage 1 bench wiring instructions |
| `hardware/components_list.txt` | Bill of materials with costs |
| `hardware/assembly_notes.txt` | 4-day build log and lessons learned |

---

## Outcomes

- Clean modular firmware structure for embedded interviews and reviews
- Local simulation workflow that can be rerun on any machine with VS Code + Wokwi + Arduino CLI
- Clear validation trail for pump activation, interlock behavior, and cooldown logic

Project by Niroop Baliji.
