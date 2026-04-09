# Local Wokwi Simulation

This folder contains the software-only digital twin used for quick validation in VS Code.

## Contents

- `diagram.json` - Wokwi circuit layout
- `sketch.ino` - standalone simulation firmware
- `wokwi.toml` - firmware and ELF runtime settings
- `libraries.txt` - libraries required for local builds
- `scripts/` - build, clean, and launch helpers
- `build/` - generated firmware artifacts

## Simulated behavior

- DHT22 temperature and humidity sensing
- Soil moisture input from an analog control
- Tank level switch for water availability
- Relay-driven pump indicator
- Serial telemetry at 115200 baud

## Build

1. Install `arduino-cli` and add it to `PATH`.
2. Install the ESP32 core and libraries:
   - `arduino-cli core update-index`
   - `arduino-cli core install esp32:esp32@3.3.7`
   - `arduino-cli lib install "DHT sensor library for ESPx"`
   - `arduino-cli lib install "DHTesp"`
3. Build with the VS Code task **Wokwi: Build Firmware** or run `scripts\build-firmware.ps1`.

The build produces:

- `build\sketch.ino.bin`
- `build\sketch.ino.elf`

## Run

1. Build the firmware first.
2. Open `simulation\wokwi` in VS Code.
3. Start **Wokwi: Start Simulator**.

## Validation checks

- Dry soil should trigger watering when the tank is available.
- Tank empty should block watering.
- Cooldown should prevent immediate retrigger.
- Serial telemetry should match the current control state.
