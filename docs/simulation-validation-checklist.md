# Simulation Validation Checklist (Digital Twin)

This checklist tracks simulation evidence quality and completion status.

## Completion Tracker

- [x] Baseline wet-soil hold (pump remains OFF)
- [x] Scenario 1: Dry soil triggers watering
- [x] Scenario 2: Tank-low safety interlock blocks watering
- [x] Scenario 3: System overview capture
- [x] Optional Scenario 4: Cooldown behavior proof

Current status: Digital-twin scenarios are captured and checklist evidence is complete.

## Baseline: Wet Soil Hold

Purpose:
- Confirm controller does not activate pump when soil is already wet.

Expected telemetry pattern:

```text
soil_raw=4095
moisture_pct=100
pump=OFF
decision=hold_soil_ok
```

Success criteria:
- Pump remains OFF through repeated loop cycles.
- Decision remains hold_soil_ok while tank is available.

## Scenario 1: Dry Soil Activates Pump

Purpose:
- Verify pump starts when moisture falls below threshold and tank has water.

Procedure:
1. Open the simulator and start serial monitoring.
2. Lower soil value until moisture percent falls below threshold.
3. Observe decision and pump state transition.

Expected output:

```text
decision=watering_start
pump=ON
```

Capture requirements:
- Include pump indicator ON.
- Include serial line showing pump=ON and decision=watering_start.
- Record timestamp and key serial lines in validation notes.

## Scenario 2: Tank-Low Safety Interlock

Purpose:
- Verify tank-empty condition overrides dry-soil watering request.

Procedure:
1. Keep dry-soil condition active.
2. Toggle tank level input to empty state.
3. Observe decision and pump state.

Expected output:

```text
decision=hold_tank_empty
pump=OFF
```

Capture requirements:
- Include tank switch state and serial output in one frame.
- Record timestamp and key serial lines in validation notes.

## Scenario 3: System Overview

Purpose:
- Provide one full-architecture screenshot for documentation context.

Procedure:
1. Reset simulator into stable state.
2. Keep all major components visible.
3. Include serial monitor if readability allows.

Capture requirements:
- Record current simulator setup and active components in validation notes.

## Optional Scenario 4: Cooldown Behavior

Purpose:
- Demonstrate anti-chatter cooldown after a watering cycle.

Expected sequence example:

```text
ACTION: watering_start
ACTION: watering_stop -> cooldown_start
ACTION: cooldown_end
```

Capture requirements:
- Show sequence context clearly in serial monitor.
- Record sequence lines with timestamps in validation notes.

## Evidence Bundle

Minimum required evidence:
- Scenario completion checklist marked for baseline + scenarios 1-3
- Serial telemetry excerpts for watering start and tank interlock hold

Recommended evidence:
- Serial telemetry excerpts for cooldown behavior

## Notes

- Digital twin evidence is acceptable for software-first validation.
- Hardware validation can be documented as Phase 2.
