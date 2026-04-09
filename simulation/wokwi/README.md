# Local Wokwi Simulation (VS Code)

This folder is a local digital twin for quick demos and validation in VS Code.

## Files
- `diagram.json`: visual circuit with a clear left-to-right signal layout
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
   - **Windows**: Download from https://arduino.cc/pro/software or use `choco install arduino-cli`
   - **macOS**: `brew install arduino-cli`
   - **Linux**: `apt install arduino-cli` or download from GitHub
2. Install ESP32 core once:
   - `arduino-cli core update-index`
   - `arduino-cli core install esp32:esp32@2.0.13`
3. Install required libraries:
   - `arduino-cli lib install "DHT sensor library for ESPx"`
   - `arduino-cli lib install "DHTesp"`
4. Build firmware:
   - **VS Code Task**: `Wokwi: Build Firmware`
   - **Or manually**: `powershell -ExecutionPolicy Bypass -File simulation\wokwi\scripts\build-firmware.ps1`
   - Output: `build/sketch.ino.bin` and `build/sketch.ino.elf`

**Note**: If build fails, rerun `Wokwi: Clean Build` and then `Wokwi: Build Firmware`.

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

## Validation Tips
- Keep simulator wiring readable and labels visible during checks.
- Use serial telemetry output to verify control decisions.
- Record key timestamps and decision lines in project validation notes.
