# Simulation Validation Checklist

This checklist records the current digital-twin evidence set.

## Status

- [x] Wet-soil hold
- [x] Dry-soil watering
- [x] Tank-empty interlock
- [x] System overview capture
- [x] Cooldown behavior

## Wet-soil hold

Purpose:
- Confirm the controller keeps the pump off when soil is already wet.

Expected telemetry:

```text
soil_raw=4095
moisture_pct=100
pump=OFF
decision=hold_soil_ok
```

Success criteria:
- Pump stays OFF across repeated loop cycles.
- Decision remains `hold_soil_ok` while tank water is available.

## Dry-soil watering

Purpose:
- Confirm watering starts when moisture drops below the threshold and the tank is available.

Procedure:
1. Open the simulator and start serial monitoring.
2. Lower the soil input until moisture falls below the threshold.
3. Confirm the pump state changes to ON.

Expected telemetry:

```text
decision=watering_start
pump=ON
```

## Tank-empty interlock

Purpose:
- Confirm tank-empty status overrides a dry-soil request.

Procedure:
1. Keep the soil dry condition active.
2. Set the tank input to empty.
3. Confirm watering is blocked.

Expected telemetry:

```text
decision=hold_tank_empty
pump=OFF
```

## System overview

Purpose:
- Capture one complete simulator state for documentation.

Procedure:
1. Reset the simulator into a stable state.
2. Keep the major components visible.
3. Include the serial monitor if it remains readable.

## Cooldown behavior

Purpose:
- Confirm the anti-chatter cooldown after watering.

Expected sequence:

```text
ACTION: watering_start
ACTION: watering_stop -> cooldown_start
ACTION: cooldown_end
```

## Evidence bundle

Required:
- Completed checklist for the scenarios above
- Serial excerpts for watering start and tank interlock hold

Recommended:
- Serial excerpt for cooldown behavior
