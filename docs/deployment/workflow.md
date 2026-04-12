# Workflow Documentation

## 1. Objective

This document gives a single, readable workflow map for simulation, hardware, and validation using the current repository layout.

## 2. End-to-end flow

1. Understand project behavior from `../../README.md` and `../architecture/architecture.md`.
2. Run simulation workflow in `simulation/wokwi/`.
3. Validate expected behavior using `../validation/simulation-validation-checklist.md`.
4. Assemble hardware using `../../hardware/wiring/build-guide.md`.
5. Flash production firmware `firmware/firmware.ino`.
6. Re-run key validation scenarios on hardware.

## 3. Simulation workflow details

- build firmware with scripts or VS Code tasks
- start simulator with VS Code task
- observe telemetry logs
- test wet, dry, tank-empty, and cooldown scenarios

## 4. Hardware workflow details

- complete wiring with common ground
- confirm relay polarity and tank polarity settings
- flash production firmware
- verify startup and safety behavior in serial logs

## 5. Validation workflow details

For each scenario, capture:

- setup condition
- observed telemetry snippets
- pass/fail result
- notes if behavior differs from expected

## 6. Troubleshooting workflow

If behavior is unexpected:

1. Verify wiring against `../../hardware/wiring/wiring-diagram.mmd`.
2. Re-check config values in `../../firmware/include/config.h`.
3. Confirm you are using the correct firmware target file.
4. Clean and rebuild simulation artifacts when needed.

## 7. Synchronization rule

When any technical value is changed in code, update all affected documentation files in the same change set.
