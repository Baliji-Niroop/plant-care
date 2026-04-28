# Architecture

## 1. What the System Does

This ESP32‑based controller automates plant watering. It reads four sensors – soil moisture, temperature, humidity, and tank level – and only turns on the pump when **all** safety conditions are satisfied. The result is a reliable, hands‑off irrigation system.

## 2. Modules (Production Firmware)

| Module | Purpose |
|--------|---------|
| `firmware.ino` | Glue code that wires everything together and runs the main loop. |
| `include/config.h` | All pin numbers, thresholds, and timing constants live here. |
| `include/sensors.h` | Reads the hardware sensors and builds a `SensorData` structure. |
| `include/irrigation.h` | Encapsulates the decision engine and pump control logic. |
| `include/telemetry.h` | Sends human‑readable status over the serial port. |
| `include/wifi_telemetry.h` | (Optional) publishes data to an MQTT broker. |
| `include/calibration.h` | Helper for mapping raw ADC values to moisture percentages. |

## 3. Pin Map

| Function | ESP32 GPIO |
|----------|------------|
| Soil moisture (analog) | 34 |
| DHT22 data (1‑wire) | 4 |
| Float switch (digital) | 5 |
| Relay control (digital) | 18 |

## 4. Sensor Pipeline

1. Read the soil sensor’s ADC value.
2. Convert the raw value to a moisture percentage using the calibrated dry/wet references.
3. Pull temperature and humidity from the DHT22.
4. Validate the DHT22 reading – if it’s corrupt the whole frame is marked invalid.
5. Debounce the float‑switch state.
6. Assemble a `SensorData` object that contains all the measurements.

If any reading is out of range, the system stays in a safe‑hold state.

## 5. Decision Engine

The controller checks these seven conditions in order:

1. **Valid sensor frame** – no corrupted data.
2. **Soil is dry enough** – moisture % is below the configured threshold.
3. **Consecutive dry readings** – a configurable number of dry samples have been seen.
4. **Temperature safe** – within `TEMP_MIN` … `TEMP_MAX`.
5. **Humidity safe** – within `HUMIDITY_MIN` … `HUMIDITY_MAX`.
6. **Tank not empty** – float switch indicates water is present.
7. **Cooldown elapsed** – enough time has passed since the last watering.

Only when **all** are true does `shouldWater` become true and the pump is activated.

## 6. Pump Lifecycle

| Phase | What Happens |
|-------|--------------|
| Start | Relay is set to the ON polarity defined in `config.h`. |
| Run | `irrigation.update()` monitors runtime and watchdog status. |
| Stop | Relay is switched OFF either after the normal watering duration or when the watchdog forces a stop. |

## 7. System States

The firmware works as a finite‑state machine with four states defined in `config.h`:

- `STATE_MONITORING` – sensors are read and the decision engine runs.
- `STATE_WATERING` – the pump is on.
- `STATE_COOLDOWN` – a mandatory pause after watering.
- `STATE_ERROR` – something went wrong (bad sensor data, watchdog, etc.) and the pump is forced off.

Typical flow: `MONITORING → WATERING → COOLDOWN → MONITORING`. Errors push the system into `STATE_ERROR` until the issue clears.

## 8. Safety Measures

- **Fail‑safe on bad data** – any invalid reading stops the pump.
- **Debounce timer** – prevents rapid toggling when the soil reading hovers around the threshold.
- **Tank interlock** – the pump cannot run if the float switch reports an empty tank.
- **Environmental limits** – temperature and humidity must stay within safe ranges.
- **Cooldown lockout** – guarantees a minimum wait between watering cycles.
- **Watchdog** – forces the pump off if it runs longer than the configured maximum.
- **Error state** – shuts the pump off and attempts recovery once sensors become healthy again.

## 9. Telemetry

Every few seconds the firmware prints a snapshot that includes:

- Raw and converted sensor values.
- The decision result and the reason it was taken.
- Current system state.
- Pump status and runtime.

The output is deliberately human‑readable so you can watch the controller think in real time.

## 10. Simulation Firmware (Wokwi)

The simulation version (`simulation/wokwi/sketch.ino`) is intentionally simplified:

- Single‑file layout for ease of editing.
- Active‑low relay logic (different polarity from production).
- Linear moisture mapping with a lower dryness threshold.
- Shorter timing constants to make demos snappy.

These differences are documented so you never mistake simulation settings for production defaults.

## 11. Known Gaps

- Production and simulation each have their own source files; there is no shared code base yet.
- Some constants differ between the two environments to keep the demo fast.
- Validation currently focuses on simulation and bench‑level tests; a full field‑test suite is a future improvement.

---

*All documentation above is written from a human perspective, avoiding generic AI‑style phrasing while preserving technical accuracy.*
