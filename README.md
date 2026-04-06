# ESP32 Plant Irrigation Controller

Compact, readable ESP32 irrigation logic with a clean module boundary and predictable runtime behavior.

## Project Layout

```text
PLANT-CARE/
├── main.ino
├── include/
│   ├── config.h
│   ├── sensors.h
│   ├── irrigation.h
│   └── telemetry.h
└── simulation/
    ├── diagram.json
    └── wokwi.toml
```

## Behavior

- Reads sensors every 2 seconds
- Waters only when all checks pass:
  - soil is dry
  - dry state is stable (debounced)
  - environment is safe
  - tank has water
  - cooldown has expired
- Pump runs for 5 seconds with watchdog protection

The code is intentionally simple: all thresholds/timing in one place, sensor acquisition isolated, irrigation logic explicit, and telemetry output easy to scan.

## Pin Mapping

| Component | GPIO |
| --- | --- |
| Soil Sensor (analog) | 34 |
| DHT11 | 4 |
| Tank Switch | 5 |
| Relay | 18 |

## Configuration

All thresholds, timing values, and pin mappings are in `include/config.h`.

## Simulation Notes

Simulation assets are in `simulation/`.  
If your Wokwi setup expects files at root, copy `simulation/diagram.json` and `simulation/wokwi.toml` to the workspace root before starting simulation.

## Hardware Notes

Install the Adafruit DHT library in Arduino IDE before upload.  
Relay behavior is configured in `include/config.h` (`RELAY_ON` / `RELAY_OFF`) so you can adapt quickly for active-low relay boards.
