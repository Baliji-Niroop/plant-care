# Simulation Validation Checklist

Use this checklist to validate only the Wokwi simulation behavior.

## Scope (read first)

This checklist is for:
- `simulation/wokwi/sketch.ino`

This checklist is **not** for:
- `firmware/firmware.ino` on physical hardware

Intentional simulation differences:

1. Relay is active-low in simulation.
2. Soil mapping is linear (`0..4095 -> 0..100`).
3. Production firmware uses calibrated soil mapping (`3950..1650 -> 0..100`).
4. Simulation threshold is lower to make demos faster.

## Scenario status

- [x] Wet-soil hold
- [x] Dry-soil watering
- [x] Tank-empty interlock
- [x] System overview capture
- [x] Cooldown behavior

## 1) Wet-soil hold

Goal:
- Pump should stay OFF when soil is wet.

Expected serial pattern:

```text
soil_raw=4095
moisture_pct=100
pump=OFF
decision=hold_soil_ok
```

Pass condition:
- Pump remains OFF over repeated loops.
- Decision stays `hold_soil_ok` while tank is available.

## 2) Dry-soil watering

Goal:
- Pump should start when soil is dry and tank is available.

Steps:
1. Open simulator and serial monitor.
2. Lower soil input until moisture is below threshold.
3. Confirm pump becomes ON.

Expected serial pattern:

```text
decision=watering_start
pump=ON
```

## 3) Tank-empty interlock

Goal:
- Tank-empty must block watering even if soil is dry.

Steps:
1. Keep dry-soil condition active.
2. Set tank switch to empty.
3. Confirm watering is blocked.

Expected serial pattern:

```text
decision=hold_tank_empty
pump=OFF
```

## 4) System overview capture

Goal:
- Capture one clean full-system screenshot for records.

Steps:
1. Reset simulator to stable state.
2. Keep major components visible.
3. Include serial monitor if readable.

## 5) Cooldown behavior

Goal:
- Confirm cooldown blocks immediate retrigger after watering.

Expected sequence:

```text
ACTION: watering_start
ACTION: watering_stop -> cooldown_start
ACTION: cooldown_end
```

## Evidence to save

Required:
- Completed checklist
- Serial excerpt for watering start
- Serial excerpt for tank-empty hold

Recommended:
- Serial excerpt for cooldown sequence

## Latest run summary

Date: 2026-04-09

- Clean build: PASS
- Firmware build: PASS
- Simulator start trigger: PASS

Notes:
- Firmware size: 292624 bytes (22%)
- Global memory: 22252 bytes (6%)
- ESP32 BT pragma messages are informational, not errors
