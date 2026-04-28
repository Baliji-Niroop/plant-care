# Architecture

## 1. System purpose

The system automates plant watering using measured sensor inputs and safety rules. It is designed to run in two contexts:

- production firmware for physical hardware in `firmware/`
- simulation firmware for Wokwi in `simulation/wokwi/`

Both contexts follow the same core intent: water only when required and safe.

## 2. High-level module layout

Production firmware is modular and split into focused files:

- `firmware/firmware.ino`: top-level orchestration and system state transitions
- `firmware/include/config.h`: pin map, thresholds, timing, and polarity constants
- `firmware/include/sensors.h`: sensor acquisition and data validity handling
- `firmware/include/irrigation.h`: irrigation decision and pump lifecycle control
- `firmware/include/telemetry.h`: serial reporting and periodic status output

## 3. Hardware signal map

Production pin assignments:

- GPIO34: soil moisture analog input
- GPIO4: DHT22 data
- GPIO5: tank float switch input
- GPIO18: relay control output

## 4. Sensor input pipeline

Each sensor cycle follows this order:

1. Read soil ADC value.
2. Convert raw value to moisture percent using calibrated dry/wet points.
3. Read temperature and humidity from DHT22.
4. Evaluate DHT read validity.
5. Read and debounce tank switch state.
6. Build one `SensorData` frame.

If temperature or humidity is invalid, the frame is marked invalid and control remains fail-safe.

## 5. Decision logic pipeline

The irrigation controller evaluates these conditions:

1. Valid sensor frame
2. Soil moisture below dry threshold
3. Required number of consecutive dry readings reached
4. Temperature within safe range
5. Humidity within safe range
6. Tank has water
7. Cooldown expired

Only if all are true, `shouldWater` becomes true.

## 6. Pump lifecycle logic

Pump control has three phases:

- start: relay set to configured ON polarity
- run: monitored continuously by `update()`
- stop: relay set to OFF after normal duration or watchdog timeout

Safety behavior:

- normal stop after `PUMP_ON_DURATION_MS`
- forced stop when runtime exceeds `PUMP_WATCHDOG_MS`
- cooldown applied after stop

## 7. System state model

Top-level system states from `config.h`:

- `STATE_MONITORING`
- `STATE_WATERING`
- `STATE_COOLDOWN`
- `STATE_ERROR`

Typical transition path:

`MONITORING -> WATERING -> COOLDOWN -> MONITORING`

Error path:

- repeated sensor failures or watchdog-triggered conditions can push the system to `STATE_ERROR`
- error handling forces pump OFF and attempts recovery when sensors become healthy again

## 8. Safety mechanisms implemented

Implemented protections in current codebase:

- fail-safe hold on invalid sensor frame
- dry-reading debounce
- tank-empty interlock
- environment range gating
- cooldown lockout
- watchdog emergency stop
- explicit error-state pump shutdown

## 9. Telemetry model

Telemetry provides periodic snapshots including:

- sensor values and validity
- decision action and reason
- current system mode
- pump status and runtime

Default serial baud rate is 115200.

## 10. Simulation architecture notes

Simulation firmware (`simulation/wokwi/sketch.ino`) is intentionally simplified:

- single-file structure
- active-low relay behavior in Wokwi
- linear moisture mapping
- lower dryness threshold and shorter timing to make demos faster

These differences are intentional and are not production settings.

## 11. Known architecture limitations in current project

Existing limitations, without introducing new assumptions:

- production and simulation use separate firmware files
- some control constants differ between simulation and production for demo speed
- validation evidence is currently oriented around simulator and bench-level verification
