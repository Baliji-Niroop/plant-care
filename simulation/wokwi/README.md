# Local Wokwi Simulation

This folder provides the software-only validation environment for quick behavior testing in VS Code.

## 1. Folder contents

- `diagram.json`: Wokwi circuit definition
- `sketch.ino`: simulation firmware (single-file)
- `wokwi.toml`: firmware/ELF paths and Wokwi license key
- `libraries.txt`: required Arduino libraries
- `scripts/`: build, clean, and simulator-start helper scripts
- `build/`: generated build outputs

## 2. What simulation models

- DHT22 temperature and humidity input
- soil moisture analog input
- tank switch state input
- relay output used as pump indicator
- serial telemetry at 115200 baud

## 3. Simulation boundary

Use this file only for simulation:

- `simulation/wokwi/sketch.ino`

Use this file for physical hardware:

- `firmware/firmware.ino`

Do not flash the simulation sketch to real hardware.

## 4. Intentional simulation differences

These differences are expected and already implemented:

1. relay control is active-low in simulation (`LOW = ON`)
2. moisture mapping is linear (`0..4095 -> 0..100`)
3. production firmware uses calibrated mapping (`3950..1650 -> 0..100`)
4. simulation dry threshold is lower (`35%`) than production (`60%`)
5. simulation cooldown is shorter for faster demos (`15000 ms` vs production `60000 ms`)
6. simulation enforces the same temperature and humidity safety window as production (`15..40 C`, `30..80%`)

## 5. Build workflow

1. Install Arduino CLI and ensure it is available in PATH.
2. Install dependencies listed in `libraries.txt`.
3. Set `wokwi_license` in `wokwi.toml`.
4. Run VS Code task `Wokwi: Build Firmware` or run `scripts/build-firmware.ps1`.

Expected outputs:

- `build/sketch.ino.bin`
- `build/sketch.ino.elf`

## 6. Start workflow

1. Build simulation firmware.
2. Run VS Code task `Wokwi: Start Simulator`.
3. Open serial monitor and observe telemetry.

## 7. Validation workflow

After startup, validate:

- wet-soil hold behavior
- dry-soil watering behavior
- tank-empty interlock behavior
- environment safety hold behavior
- cooldown behavior

Use `../../docs/validation/simulation-validation-checklist.md` for the formal scenario checklist.
