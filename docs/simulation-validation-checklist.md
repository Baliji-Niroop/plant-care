# Simulation Validation Checklist (Digital Twin)

The project is in digital twin validation.
Repeated telemetry with pump OFF during wet-soil conditions is expected.

## Current Baseline (Wet Soil Hold)

Expected telemetry pattern:

```text
soil_raw=4095
moisture_pct=100
pump=OFF
decision=hold_soil_ok
```

What this confirms:
- Soil is interpreted as already wet.
- Controller correctly holds pump OFF.
- Baseline logic is working.

## Proof Scenario 1: Dry Soil Activates Pump

Goal:
- Force moisture below threshold and verify watering decision.

Steps:
1. In Wokwi, click the soil analog resistor/sensor element.
2. Reduce value until moisture drops below threshold.
3. Watch serial output for decision change.

Expected result:

```text
decision=watering_start
pump=ON
```

Screenshot evidence:
- Capture when all are visible at once:
  - Pump indicator (blue LED) ON
  - Serial output shows `pump=ON`
  - Dry soil condition is visible in simulator
- Save image as `docs/images/pump-active.png`

## Proof Scenario 2: Tank-Low Safety Interlock

Goal:
- Prove safety override blocks pump even during dry soil.

Steps:
1. Keep dry-soil condition active.
2. Toggle tank level switch to OFF (tank low).
3. Observe serial output and pump state.

Expected result:

```text
decision=hold_tank_empty
pump=OFF
```

Screenshot evidence:
- Capture serial + circuit state showing safety hold.
- Save image as `docs/images/safety-interlock.png`

## Proof Scenario 3: System Overview

Goal:
- Capture one clean full-system view for architecture context.

Steps:
1. Reset simulator to a clean stable state.
2. Keep all major components visible in one frame.
3. Include Serial Monitor if possible.

Screenshot evidence:
- Save image as `docs/images/system-overview.png`

## Optional Proof Scenario 4: Cooldown Behavior

Goal:
- Demonstrate anti-chatter control after a watering cycle.

Expected serial sequence (example):

```text
Pump started
Pump stopped
Cooldown active
Cooldown finished
```

Screenshot evidence:
- Save image as `docs/images/cooldown-proof.png`

## Portfolio Evidence Pack

Minimum proof set:
- `docs/images/system-overview.png`
- `docs/images/pump-active.png`
- `docs/images/safety-interlock.png`

Recommended addition:
- `docs/images/cooldown-proof.png`

## Notes

- Physical hardware is not required at this stage.
- Digital twin validation is valid engineering evidence for firmware logic and safety behavior.
- Hardware prototype testing can be treated as Phase 2 validation.
