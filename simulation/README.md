# Simulation Framework

This is where you can run and test the system without any hardware. It uses Wokwi's ESP32 simulator, which is actually pretty good for this kind of embedded logic—you get a virtual breadboard, the sensors respond realistically enough, and you can see everything the controller "thinks" via serial output.

The simulation is useful if you're learning how the system works, or if you've modified the logic and want to verify it still makes sense before flashing to real hardware.

## What's Inside

`wokwi/` contains the complete simulation project:
- `sketch.ino` — The firmware that runs in the simulator (similar to production firmware but with faster timings)
- `diagram.json` — The circuit: shows which pins connect where
- `wokwi.toml` — Configuration, including your Wokwi API token
- `README.md` — Detailed explanation of what the simulation does and doesn't do

## How to Use It

**Prerequisites:** You need Arduino CLI and Wokwi CLI installed, plus a free Wokwi account with an API token.

**To build and run:**

1. Open the project in VS Code
2. Run the task `Wokwi: Build Firmware` — compiles the sketch for ESP32
3. Run the task `Wokwi: Start Simulator` — launches the simulator in your browser
4. Watch the circuit respond in real-time. If you put the virtual soil sensor in "dry," the pump will turn on. If you flip the tank switch to "empty," the pump will stop.

**To validate behavior:**

Follow the scenarios in `docs/06_validation.md`. The simulation is useful specifically because you can test failure modes—set tank to empty, change temperature to freezing, etc.—without breaking real hardware.

## What This Proves

Simulation validates that:
- **Dry soil triggers watering** — When soil ADC drops below threshold, pump turns on
- **Wet soil doesn't trigger** — When soil is wet, pump stays off even if everything else looks good
- **Tank empty stops the pump** — Float switch = empty blocks watering hard
- **Cooldown prevents rapid cycling** — One watering cycle finishes, then the next attempt is blocked for 60 seconds
- **Environmental bounds work** — Setting temperature outside range blocks watering

Serial output at 115200 baud shows every sensor reading and decision, so you can trace exactly what the system thinks at any moment.

## Important: Simulation ≠ Production

The simulation firmware is different from the production firmware on purpose:

- **Timers are faster** — Cooldown is 10 seconds instead of 60, debounce is faster. This makes testing quicker.
- **Sensor values are simplified** — The simulator doesn't replicate every quirk of a real capacitive sensor
- **Relay polarity might differ** — We use active-low in simulation but active-high in hardware

The simulation is for learning and validating the *logic*. Once you're confident the decision-making is correct, you move to hardware with the production firmware.

🔴 **Critical warning:** Don't flash `simulation/wokwi/sketch.ino` to a real ESP32. It's designed to run in Wokwi only. Use `firmware/firmware.ino` for hardware.
