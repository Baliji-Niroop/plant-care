# Firmware

This folder contains production firmware for physical ESP32 hardware.

## Structure

- `firmware.ino`: production entry point
- `include/config.h`: pin map, thresholds, timing, and polarity constants
- `include/sensors.h`: sensor acquisition and health logic
- `include/irrigation.h`: irrigation decision and pump safety control
- `include/telemetry.h`: serial telemetry reporting

Use this firmware for hardware flashing.
Do not flash the simulation sketch from `simulation/wokwi/sketch.ino`.
