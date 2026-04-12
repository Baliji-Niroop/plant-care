# Firmware

This folder contains production firmware for physical ESP32 hardware.

## Structure

- `firmware.ino`: production entry point
- `include/config.h`: pin map, thresholds, timing, and polarity constants
- `include/sensors.h`: sensor acquisition and health logic
- `include/irrigation.h`: irrigation decision and pump safety control
- `include/telemetry.h`: serial telemetry reporting

## What this firmware handles

- reads soil moisture, DHT22 temperature/humidity, and tank switch state
- applies 3-layer safety logic before activating the relay and pump
- enforces cooldown and watchdog protections between irrigation cycles
- emits serial telemetry for validation and troubleshooting

## Build target

Use `esp32:esp32:esp32` when compiling with Arduino CLI for this repository.

Use this firmware for hardware flashing.
Do not flash the simulation sketch from `simulation/wokwi/sketch.ino`.
