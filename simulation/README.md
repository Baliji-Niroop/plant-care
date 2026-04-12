# Simulation

This folder contains simulation-specific resources.

## Structure

- `wokwi/`: local Wokwi simulation project, scripts, and simulator configuration

## What to run

1. Configure `simulation/wokwi/wokwi.toml` with your `wokwi_license`.
2. Run `Wokwi: Build Firmware` from VS Code tasks.
3. Run `Wokwi: Start Simulator` to launch the local digital twin.

## What this validates

- dry vs wet watering behavior
- tank-empty interlock behavior
- cooldown lockout behavior
- serial telemetry visibility at 115200 baud

Start from `simulation/wokwi/README.md` for the simulation workflow.
