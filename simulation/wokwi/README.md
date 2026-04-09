# Local Wokwi Simulation (VS Code)

This folder is a local digital twin for quick demos and validation in VS Code.

## Files
- `diagram.json`: visual circuit (left-to-right layout for clean screenshots)
- `sketch.ino`: single-file ESP32 simulation firmware
- `wokwi.toml`: Wokwi binary/ELF runtime config
- `libraries.txt`: explicit library list for reliable local builds
- `scripts/build-firmware.ps1`: builds `build/sketch.ino.bin` + `build/sketch.ino.elf`
- `scripts/clean-build.ps1`: removes build artifacts
- `scripts/start-simulator.ps1`: starts simulator via VS Code CLI when available
- `build/`: generated firmware outputs

## Simulated Subsystems
- DHT22 temperature/humidity sensor
- Soil moisture analog signal (potentiometer)
- Tank level switch (water available / empty)
- Relay-driven pump indicator LED
- Serial telemetry output at 115200 baud

## Behavior Implemented
- Moisture threshold watering automation
- Safe timed watering stop
- Watchdog stop protection
- Cooldown delay to avoid rapid pump cycling
- Tank-empty block for watering safety
- Structured serial telemetry logs

## Local Build (Required for firmware/ELF mode)
1. Install `arduino-cli` and make sure it is available in `PATH`.
2. Install ESP32 core once:
	- `arduino-cli core update-index`
	- `arduino-cli core install esp32:esp32`
3. Install required library once:
	- `arduino-cli lib install "DHT sensor library for ESPx"`
4. Build firmware:
	- Run VS Code task: `Wokwi: Build Firmware`
	- or run: `powershell -ExecutionPolicy Bypass -File simulation\wokwi\scripts\build-firmware.ps1`

## Run Locally in VS Code
1. Build first so `build/sketch.ino.bin` and `build/sketch.ino.elf` exist.
2. Open this folder path in the workspace: `simulation\wokwi`.
3. Start simulator:
	- VS Code task: `Wokwi: Start Simulator`
	- or `F1` -> `Wokwi: Start Simulator`

## Quick Demo Checks
1. Turn soil potentiometer down to simulate dry soil.
2. Keep the tank switch ON (water available).
3. Confirm relay/pump LED turns on, then auto-stops after watering duration.
4. Confirm cooldown prevents immediate retrigger.
5. Set tank switch OFF and confirm watering is blocked.

## Screenshot Tips
- Keep the simulator zoom around 90-110%.
- Use the left-to-right signal flow in `diagram.json` for publication-friendly captures.
- Capture both circuit view and serial telemetry to show control decisions.
