# Simulation Validation Checklist

This checklist validates Wokwi simulation behavior for `simulation/wokwi/sketch.ino`.

It does not validate physical hardware behavior from `firmware/firmware.ino`.

## Scope and expected context

Before running scenarios:

1. Build simulation firmware successfully.
2. Start Wokwi simulator.
3. Open serial monitor at 115200 baud.

## Simulation notes (intentional differences)

- relay behavior is active-low in simulation
- soil mapping is linear (`0..4095 -> 0..100`)
- production firmware uses calibrated mapping (`3950..1650 -> 0..100`)
- simulation dry threshold is lower for faster demo cycles

## Scenario checklist

- [x] Wet-soil hold
- [x] Dry-soil watering
- [x] Tank-empty interlock
- [x] Environment safety hold
- [x] System overview capture
- [x] Cooldown behavior

## 1. Wet-soil hold

Objective: pump remains OFF when soil is not dry.

Expected serial pattern:

```text
soil_raw=4095
moisture_pct=100
pump=OFF
decision=hold_soil_ok
```

Pass criteria:

- pump stays OFF over repeated loops
- decision remains `hold_soil_ok` when tank is available

## 2. Dry-soil watering

Objective: pump turns ON when soil is dry and tank is available.

Steps:

1. Lower soil input until moisture drops below threshold.
2. Observe decision and pump fields.

Expected serial pattern:

```text
decision=watering_start
pump=ON
```

Pass criteria:

- watering start appears in logs
- pump status transitions to ON

## 3. Tank-empty interlock

Objective: tank-empty state blocks watering even under dry soil.

Steps:

1. Keep dry-soil condition active.
2. Set tank switch to empty.
3. Observe decision and pump fields.

Expected serial pattern:

```text
decision=hold_tank_empty
pump=OFF
```

Pass criteria:

- watering does not start
- decision remains `hold_tank_empty` while tank is empty

## 4. Environment safety hold

Objective: out-of-range temperature or humidity blocks watering even when soil is dry and tank is available.

Steps:

1. Keep dry-soil condition active.
2. Keep tank switch in the water-available state.
3. Set temperature below `15 C`, above `40 C`, or humidity outside `30..80%`.
4. Observe decision and pump fields.

Expected serial pattern:

```text
decision=hold_env_unsafe
pump=OFF
```

Pass criteria:

- watering does not start
- decision remains `hold_env_unsafe` while the environment is out of range

## 5. System overview capture

Objective: capture one clean full-system evidence image.

Steps:

1. Reset to stable simulator state.
2. Keep circuit and serial monitor visible.
3. Capture a screenshot with readable telemetry.

## 6. Cooldown behavior

Objective: verify cooldown prevents immediate retrigger.

Expected sequence:

```text
ACTION: watering_start
ACTION: watering_stop -> cooldown_start
ACTION: cooldown_end
```

Pass criteria:

- no immediate restart during active cooldown
- restart allowed only after `cooldown_end`

## Evidence to archive

Required:

- completed checklist
- serial excerpt for watering start
- serial excerpt for tank-empty hold

Recommended:

- serial excerpt showing full cooldown sequence
- one full simulator screenshot

## Latest recorded run

Date: 2026-04-09

- clean build: PASS
- firmware build: PASS
- simulator start trigger: PASS

Reference notes:

- firmware size: 292624 bytes (22%)
- global memory: 22252 bytes (6%)
- ESP32 BT pragma messages are informational
