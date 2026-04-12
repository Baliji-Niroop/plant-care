# ESP32 Smart Plant Care

An ESP32-based smart irrigation project that waters plants only when soil is dry and operating conditions are safe.

This repository is organized for both learning and engineering review:

- beginner-friendly simulation workflow
- hardware-ready firmware workflow
- documented safety logic and validation evidence

## Problem statement

Manual watering is inconsistent. Plants are often overwatered or underwatered because moisture level, environmental condition, and tank availability are not checked together before pumping.

## Why smart irrigation is needed

Smart irrigation reduces avoidable watering errors by making the pump decision from measured inputs instead of guesswork. This project focuses on a small, practical implementation:

- soil moisture indicates when watering may be needed
- temperature and humidity are checked for safe conditions
- tank level interlock prevents dry running
- cooldown and watchdog protections reduce pump stress

## What this system does

The controller reads four inputs and decides whether to water:

1. Soil moisture (analog)
2. Temperature (DHT22)
3. Humidity (DHT22)
4. Tank water availability (float switch)

The pump relay is activated only when all decision conditions pass.

## Required components

Only components already used by this repository are listed:

- ESP32 DevKit V1
- Capacitive soil moisture sensor
- DHT22 temperature and humidity sensor
- Float switch (tank level)
- 1-channel relay module
- 5V DC mini pump
- External 5V supply for pump and relay
- Common wiring accessories (breadboard, jumpers, tubing)

For cost and sourcing notes, see `hardware/components/bom.txt`.

## Working principle

### Sensor input flow

1. ESP32 reads soil ADC on GPIO34.
2. ESP32 reads temperature and humidity from DHT22 on GPIO4.
3. ESP32 reads tank switch state on GPIO5.
4. Sensor frame is marked valid or invalid.

### Decision logic flow

Watering is allowed only when:

- sensor frame is valid
- soil is below dry threshold
- dry reading debounce has passed
- temperature and humidity are within configured limits
- tank reports water available
- cooldown has expired

If any condition fails, action is HOLD.

### Pump activation flow

When a watering decision is true:

1. Relay output is set to ON polarity.
2. Pump runs for configured duration.
3. Pump stops and cooldown starts.
4. During run, watchdog can force emergency stop if runtime exceeds limit.

## Implemented safety checks

- fail-safe hold on invalid sensor data
- tank-empty interlock
- debounce for dry-soil decision
- environment range check (temperature and humidity)
- cooldown lockout between cycles
- watchdog hard stop for pump runtime
- error state handling with forced pump OFF

## Simulation and hardware workflows

### Simulation workflow (recommended first)

Use the local Wokwi setup in `simulation/wokwi/`.

1. Install Arduino CLI and required core/libraries.
2. Set `wokwi_license` in `simulation/wokwi/wokwi.toml`.
3. Run task: `Wokwi: Build Firmware`.
4. Run task: `Wokwi: Start Simulator`.
5. Validate behavior with `docs/validation/simulation-validation-checklist.md`.

### Real hardware workflow

1. Assemble hardware using `hardware/wiring/build-guide.md`.
2. Flash only `firmware/firmware.ino`.
3. Verify relay and tank polarity settings in `firmware/include/config.h`.
4. Run scenario tests (dry, wet, tank empty, cooldown).

Important: `simulation/wokwi/sketch.ino` is simulation-only and must not be flashed to physical hardware.

## Calibration workflow

Production firmware uses calibrated soil mapping values:

- dry reference: 3950
- wet reference: 1650

Calibration guidance is documented in `docs/calibration/calibration.md`.

## Validation process and expected outputs

Validation is scenario-based and uses serial telemetry as evidence.

Expected outcomes:

- wet soil keeps pump OFF
- dry soil with safe conditions starts watering
- tank empty blocks watering
- cooldown prevents immediate retrigger
- watchdog prevents excessive runtime

Use `docs/validation/simulation-validation-checklist.md` for structured checks.

## Repository structure

| Path | Purpose |
|---|---|
| `firmware/` | Production firmware and headers for physical ESP32 hardware |
| `simulation/wokwi/` | Local Wokwi simulation sketch, circuit, and helper scripts |
| `docs/architecture/` | System architecture documentation |
| `docs/calibration/` | Calibration and threshold guidance |
| `docs/deployment/` | Deployment and workflow documentation |
| `docs/validation/` | Validation checklists and expected outputs |
| `docs/troubleshooting/` | Practical prototype troubleshooting notes |
| `hardware/wiring/` | Wiring references and bench build guide |
| `hardware/components/` | Hardware bill of materials |
| `hardware/images/` | Hardware photos and captures |
| `assets/` | Shared diagrams, screenshots, and demo images |
| `.github/` | GitHub templates and repository automation metadata |

## Known limitations

Current repository limitations already present in code/docs:

- simulation uses different relay polarity than production
- simulation uses simplified moisture mapping and faster timing values
- simulation threshold differs from production threshold for demo speed
- current validation artifacts are focused on bench and simulator scenarios

These are intentional and documented in `simulation/wokwi/README.md`.

## Documentation map

- `docs/architecture/architecture.md`: system modules, signal flow, state behavior
- `docs/calibration/calibration.md`: soil calibration and threshold verification
- `docs/deployment/deployment.md`: simulation and hardware deployment steps
- `docs/deployment/workflow.md`: end-to-end workflow map
- `docs/README.md`: documentation folder index
- `CONTRIBUTING.md`: contribution and repository quality standards
- `firmware/README.md`: firmware folder quick index
- `simulation/README.md`: simulation folder quick index
- `hardware/README.md`: hardware design reference
- `hardware/wiring/build-guide.md`: assembly and bench verification
- `docs/validation/simulation-validation-checklist.md`: simulation test checklist
