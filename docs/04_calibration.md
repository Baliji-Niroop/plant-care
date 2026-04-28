# Calibration Guide

## 1. Purpose

This document defines how calibration is used in the current repository and how to verify that configured values remain valid.

It does not change firmware logic.

## 2. Current production calibration values

From `firmware/include/sensors.h`:

- dry raw reference: 3950
- wet raw reference: 1650

These values map raw ADC to moisture percentage in production firmware.

## 3. Moisture conversion behavior

Production mapping logic:

- raw near dry reference maps toward 0%
- raw near wet reference maps toward 100%
- final value is constrained between 0 and 100

Dryness decision threshold is defined separately in `firmware/include/config.h`:

- `SOIL_DRY_THRESHOLD_PERCENT = 60`

## 4. Why calibration matters

Without calibration, raw ADC values are difficult to compare across soil conditions. Using dry and wet references creates a consistent moisture percentage scale for decision logic.

## 5. Practical verification workflow

Use this workflow to confirm existing calibration still makes sense:

1. Keep sensor in dry-air condition and note typical raw value.
2. Keep sensor in wet reference condition and note typical raw value.
3. Compare observed values against configured references.
4. Confirm mapped moisture percentages are reasonable.
5. Confirm watering trigger behavior matches threshold expectations.

## 6. Validation checks to run after verification

- wet condition should produce HOLD behavior
- dry condition should produce watering decision when other safety checks pass
- tank-empty should block watering regardless of moisture

Refer to `../validation/simulation-validation-checklist.md` for simulation-side scenario structure.

## 7. Simulation vs production calibration note

Simulation uses a simplified linear mapping in `simulation/wokwi/sketch.ino` for fast demos. Production calibration values above remain the source of truth for real hardware behavior.

## 8. Related configuration values

Configuration values affecting watering sensitivity and cadence are in `firmware/include/config.h`:

- `SOIL_DRY_THRESHOLD_PERCENT`
- `DECISION_DEBOUNCE_READINGS`
- `SENSOR_READ_INTERVAL_MS`
- `PUMP_ON_DURATION_MS`
- `PUMP_COOLDOWN_MS`
- `PUMP_WATCHDOG_MS`

## 9. Known limitation

Current project stores calibration references as compile-time constants. Runtime calibration persistence is not part of the existing implementation.
