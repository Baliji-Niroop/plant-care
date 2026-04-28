# Deployment Guide

## 1. Scope

This guide covers how to run the project in:

- local Wokwi simulation
- physical ESP32 hardware

It preserves the current repository workflow and settings.

## 2. Prerequisites

- VS Code
- Arduino CLI
- ESP32 core version `esp32:esp32@3.3.7`
- Required libraries:
  - `DHT sensor library for ESPx`
  - `DHTesp`

## 3. Simulation deployment (Wokwi)

### 3.1 Configure Wokwi

Edit `simulation/wokwi/wokwi.toml`:

- set `wokwi_license = "YOUR_WOKWI_LICENSE_KEY"`

### 3.2 Build firmware

Use one of the following:

- VS Code task: `Wokwi: Build Firmware`
- script: `simulation/wokwi/scripts/build-firmware.ps1`

Expected build artifacts:

- `simulation/wokwi/build/sketch.ino.bin`
- `simulation/wokwi/build/sketch.ino.elf`

### 3.3 Start simulator

Use one of the following:

- VS Code task: `Wokwi: Start Simulator`
- command palette action if CLI trigger is unavailable

### 3.4 Validate behavior

Run checklist scenarios in `../validation/simulation-validation-checklist.md`.

## 4. Hardware deployment (ESP32)

### 4.1 Assemble hardware

Follow:

- `hardware/wiring/build-guide.md`
- `hardware/README.md`

### 4.2 Confirm production firmware target

Flash only:

- `firmware/firmware.ino`

Do not flash:

- `simulation/wokwi/sketch.ino`

### 4.3 Confirm configuration before flashing

In `firmware/include/config.h`, verify:

- relay polarity: `RELAY_ON`, `RELAY_OFF`
- tank switch settings: `TANK_SWITCH_PIN_MODE`, `TANK_WATER_PRESENT_LEVEL`
- thresholds and timings as required for your setup

### 4.4 Post-flash validation

Confirm serial telemetry at 115200 and run these scenarios:

1. Wet soil hold
2. Dry soil watering
3. Tank-empty interlock
4. Cooldown behavior
5. Watchdog safety behavior

## 5. Build and clean helper scripts

Available scripts in `simulation/wokwi/scripts/`:

- `build-firmware.ps1`: compile and stage simulation firmware artifacts
- `clean-build.ps1`: remove generated build artifacts and restore `.gitkeep`
- `start-simulator.ps1`: best-effort simulator start trigger through VS Code

## 6. Expected deployment outputs

Simulation deployment is successful when:

- build artifacts are generated
- simulator starts with no blocking configuration errors
- serial logs show expected decision/action transitions

Hardware deployment is successful when:

- firmware boots without reset loops
- pump remains off at startup
- watering occurs only under valid safe conditions
- all safety interlocks remain active

## 7. Known deployment limitations

- Wokwi relay behavior is active-low and differs from common hardware relay defaults.
- Simulation thresholds and timing are intentionally faster than production values.
- `start-simulator.ps1` depends on VS Code CLI availability in PATH.
