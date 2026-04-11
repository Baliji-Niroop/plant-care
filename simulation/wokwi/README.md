# Local Wokwi Simulation

This folder is the software-only test environment for quick checks in VS Code.

## Files in this folder

- `diagram.json`: Wokwi circuit connections
- `sketch.ino`: simulation firmware (single-file)
- `wokwi.toml`: simulator config (firmware path, elf path, license key)
- `libraries.txt`: required Arduino libraries
- `scripts/`: build/clean/start helper scripts
- `build/`: generated build outputs

## What the simulation includes

- DHT22 temperature + humidity
- Soil moisture analog input
- Tank switch input
- Relay output (pump indicator)
- Serial logs at 115200 baud

## Important: simulation is not hardware firmware

`simulation/wokwi/sketch.ino` is tuned for simulation speed and behavior demos.
Real hardware uses `firmware/firmware.ino`.

Key differences:

1. Relay logic in simulation is active-low (`LOW = ON`) because of Wokwi relay behavior.
2. Moisture mapping in simulation is linear (`0..4095 -> 0..100`).
3. Production firmware uses calibrated mapping (`3950..1650 -> 0..100`).
4. Simulation dry threshold is lower (`35%`) than production (`60%`).

Do not flash the simulation sketch to physical ESP32 hardware.

## Build steps

1. Install `arduino-cli` and add it to `PATH`.
2. Install dependencies:
   - `arduino-cli core update-index`
   - `arduino-cli core install esp32:esp32@3.3.7`
   - `arduino-cli lib install "DHT sensor library for ESPx"`
   - `arduino-cli lib install "DHTesp"`
3. Add your Wokwi key in `wokwi.toml`:
   - `wokwi_license = "YOUR_WOKWI_LICENSE_KEY"`
4. Build using VS Code task **Wokwi: Build Firmware**  
   or run `scripts\build-firmware.ps1`.

Build output:

- `build\sketch.ino.bin`
- `build\sketch.ino.elf`

## Run steps

1. Build first.
2. Open `simulation\wokwi` in VS Code.
3. Start **Wokwi: Start Simulator**.

## Basic checks after start

- Dry soil should start watering when tank is available.
- Tank empty should block watering.
- Cooldown should stop immediate retrigger.
- Serial output should match current state.
