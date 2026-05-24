# Calibration Guide

## 1. Purpose

This document defines how soil moisture sensor calibration works in the ESP32 Smart Plant Care System, explaining the non-volatile storage (NVS) runtime calibration process, and how to verify and update references.

## 2. Interactive Runtime Calibration (NVS)

Unlike basic hobbyist projects that hardcode sensor values, this firmware features an **interactive, runtime calibration system** via PlatformIO/Arduino Serial Monitor. 

During the first 10 seconds of startup, the system opens a calibration window. It checks ESP32 non-volatile storage (NVS) under the namespace `"plantcare"` for custom values. If none are found, it falls back to the default factory values:

- **Default Dry Reference:** `3950` (measured in dry air)
- **Default Wet Reference:** `1650` (measured in saturated soil)

### How to Calibrate Your Sensor:
1. Connect the ESP32 to your PC and open the Serial Monitor (Baud: `115200`).
2. Press the **EN/RST** button on the ESP32 to reset the board.
3. You will see a 10-second prompt:
   ```text
   =========================================
    Calibration Mode Active (10 seconds)    
   =========================================
    -> Send 'D' via Serial to save current reading as DRY reference.
    -> Send 'W' via Serial to save current reading as WET reference.
    [Current Status] DRY: 3950, WET: 1650
   ```
4. **Calibrate Dry:** Keep the capacitive sensor in dry air, type `D` (or `d`) in the serial input, and press Enter. The current raw analog reading will be written to NVS as the new dry reference.
5. **Calibrate Wet:** Insert the sensor up to the indicator line into fully saturated soil (or a cup of water), type `W` (or `w`) in the serial input, and press Enter. This will write the reading to NVS as the wet reference.
6. The window will close after 10 seconds, and the ESP32 will boot into normal monitoring using your newly saved calibration.

## 3. Moisture Conversion Behavior

The mapping logic converts the raw analog reading to a readable percentage (0% to 100% moisture) using the calibrated references:

- Raw readings near the **dry reference** map toward `0%`.
- Raw readings near the **wet reference** map toward `100%`.
- The calculated value is constrained between `0.0f` and `100.0f` to prevent math underflows/overflows.

The dryness decision threshold is defined in `firmware/include/config.h`:
- `SOIL_DRY_THRESHOLD_PERCENT = 60` (moisture below this level triggers the pump checks).

## 4. Practical Verification Workflow

To verify that your sensor calibration is accurate over time:
1. Hold the sensor in open air; the serial telemetry should output `moisture_pct=0.0`.
2. Fully submerge the active sensing area in water/wet soil; the serial telemetry should output `moisture_pct=100.0`.
3. If the telemetry reads significantly different (e.g., negative values or values capped at 0/100 too early), repeat the **Interactive Runtime Calibration** workflow above.

## 5. Validation Checks

After calibrating, confirm the following behaviors:
- **Wet Soil:** Insert probe into wet soil. Verify that telemetry shows pump status is `OFF` and the decision status is `Soil moisture is fine`.
- **Dry Soil:** Pull probe out into dry air. Verify that pump starts (if tank has water and cooldown has elapsed).
- **Empty Tank:** With dry soil, lift the float switch (empty state). Verify the pump turns off immediately and decision changes to `Tank is empty`.

Refer to [docs/06_validation.md](06_validation.md) for step-by-step validation procedures.

## 6. Simulation vs. Production Note

* **Simulation (`simulation/wokwi/sketch.ino`):** Uses a simplified linear mapping (`0-4095` maps to `0-100%`) with a lower threshold (`35%`) to make browser testing rapid. It does not utilize NVS.
* **Production (`firmware/firmware.ino`):** Uses the calibrated NVS-backed mapping described above.

## 7. Related Configuration Parameters

Configurable limits affecting calibration and sensitivity are located in `firmware/include/config.h`:
- `SOIL_DRY_THRESHOLD_PERCENT`
- `DECISION_DEBOUNCE_READINGS`
- `SENSOR_READ_INTERVAL_MS`
