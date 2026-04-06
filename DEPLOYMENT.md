# Deployment & Field Testing Guide

> **Purpose**: Transition from simulation to real-world operation with confidence.

---

## Pre-Deployment Checklist

Before deploying with a live plant, verify each subsystem independently:

### Hardware Verification

| Component | Test | Pass Criteria |
|-----------|------|---------------|
| ESP32 | Boot and serial output | Prints "Boot complete" message |
| Soil sensor | Touch probes with wet finger | Reading increases |
| DHT11 | Breathe on sensor | Temperature/humidity change |
| Float switch | Tilt up and down | Tank status changes in serial |
| Relay | Trigger via code | Audible click, LED on relay |
| Pump | Manual 5-second test | Water flows through tubing |

### Software Verification

| Feature | Test | Pass Criteria |
|---------|------|---------------|
| Decision logic | Force dry soil reading | "Ready to water" decision |
| Cooldown | Trigger watering cycle | Cooldown timer starts |
| Watchdog | Set pump duration > 10s | Emergency stop at 10s |
| Error recovery | Disconnect DHT briefly | ERROR state, then recovery |
| Telemetry | Run for 1 minute | Regular status updates |

---

## Deployment Procedure

### Step 1: Prepare Plant and Reservoir

1. **Water plant normally** before first deployment
2. Let soil settle to natural moisture level (wait 24-48 hours)
3. Fill water reservoir completely
4. Verify float switch shows "Tank: available"

### Step 2: Position Sensors

```
                    IDEAL SENSOR PLACEMENT
                    ─────────────────────

    ┌───────────────────────────────────────┐
    │              PLANT POT                │
    │                                       │
    │         🌿 PLANT FOLIAGE              │
    │              │                        │
    │              │                        │
    │   ══════════════════════════ SOIL     │
    │         │    │    │                   │
    │    ┌────┤    │    │                   │
    │    │    │    │    │                   │
    │   SOIL  │  ROOT   │                   │
    │  SENSOR │  ZONE   │                   │
    │    │    │    │    │                   │
    │    └────┘    │    │                   │
    │              │    │                   │
    └───────────────────────────────────────┘
                   │
                   │ Tubing outlet above soil
                   ▼ (prevents soil disturbance)
              ════════
              │ WATER │
              │ OUTPUT│
              ════════

    DHT11: Mount nearby but shaded from direct sun
    Float: Inside reservoir, 2cm above pump intake
```

**Sensor placement tips**:
- Soil sensor: Insert at 45° angle, 3-5cm deep, near pot center
- DHT11: Away from heat sources, not directly above wet soil
- Water outlet: Above soil surface, angled toward root zone

### Step 3: First Live Test

1. **Start monitoring mode**: Power on, verify serial output
2. **Wait for natural dry cycle**: Let soil dry to trigger watering
3. **Observe first watering**: Verify pump runs ~5 seconds
4. **Check water delivery**: Confirm soil receives water
5. **Monitor cooldown**: Verify 60-second cooldown activates

### Step 4: 24-Hour Validation

Run unattended for 24 hours while checking periodically:

| Time | Check | Action if Failed |
|------|-------|------------------|
| +1 hour | System still running | Check power connection |
| +4 hours | Reasonable sensor readings | Review calibration |
| +8 hours | At least 1 watering cycle (if soil dried) | Lower dry threshold |
| +12 hours | No error states | Review sensor placement |
| +24 hours | Plant looks healthy | Success! |

---

## Performance Optimization

### Tuning for Your Environment

After 24-hour validation, observe patterns and adjust:

#### Too Many Watering Cycles

**Symptom**: Pump activates more than 4-6 times per day  
**Possible causes**:
- Dry threshold too high
- Pump duration too short (not saturating root zone)
- High temperature causing rapid evaporation

**Solutions**:
```cpp
// Lower the trigger threshold
constexpr int SOIL_DRY_THRESHOLD_PERCENT = 55;  // Was 60

// Or increase pump duration
constexpr unsigned long PUMP_ON_DURATION_MS = 7000UL;  // Was 5000
```

#### Too Few Watering Cycles

**Symptom**: Soil visibly dry but system not watering  
**Possible causes**:
- Dry threshold too low
- Sensor reading incorrectly (calibration issue)
- Sensor in permanently wet pocket

**Solutions**:
```cpp
// Raise the trigger threshold
constexpr int SOIL_DRY_THRESHOLD_PERCENT = 65;  // Was 60
```

Or re-position sensor to dryer area of pot.

#### Overwatering Signs

**Symptoms**: 
- Yellow leaves
- Soggy soil for days
- Root rot smell

**Solutions**:
- Increase cooldown period: `PUMP_COOLDOWN_MS = 120000UL` (2 minutes)
- Decrease pump duration: `PUMP_ON_DURATION_MS = 3000UL`
- Lower dry threshold (less sensitive to dry soil)

#### Underwatering Signs

**Symptoms**:
- Wilting leaves
- Soil pulling away from pot edges
- Light pot weight

**Solutions**:
- Decrease cooldown period: `PUMP_COOLDOWN_MS = 30000UL` (30 seconds)
- Increase pump duration: `PUMP_ON_DURATION_MS = 8000UL`
- Raise dry threshold (more sensitive to dry soil)

---

## Long-Term Maintenance

### Weekly Tasks

| Task | Procedure | Time |
|------|-----------|------|
| Refill reservoir | Top up before float triggers | 2 min |
| Visual inspection | Check wires, sensor positions | 1 min |
| Serial spot-check | Open monitor, verify readings | 2 min |

### Monthly Tasks

| Task | Procedure | Time |
|------|-----------|------|
| Clean soil sensor | Wipe probes with damp cloth | 5 min |
| Check pump flow rate | Measure output, compare to baseline | 5 min |
| Verify calibration | Air/water readings still accurate | 10 min |
| Clean reservoir | Remove algae/sediment | 10 min |

### Seasonal Tasks

| Task | Procedure |
|------|-----------|
| Adjust thresholds | Higher for summer, lower for winter |
| Check tubing | Replace if kinked or discolored |
| Inspect float switch | Ensure free movement |
| Review logs | Identify trends over season |

---

## Troubleshooting Field Issues

### "System waters correctly but plant still suffers"

**Possible causes**:
1. **Water not reaching roots**: Outlet positioned poorly
2. **Soil hydrophobic**: Dry peat repels water initially
3. **Root-bound plant**: Water runs around, not through, root ball
4. **Drainage issue**: Water sitting at bottom causing rot

**Solutions**:
- Reposition water outlet closer to stem base
- Pre-wet hydrophobic soil manually
- Repot root-bound plants
- Add drainage holes if missing

### "Erratic sensor readings in field"

**Possible causes**:
1. **Temperature fluctuation**: Affects capacitive readings
2. **Electromagnetic interference**: Pump noise coupling
3. **Corroded connections**: Humidity ingress
4. **Sensor degradation**: Capacitive sensors last 1-2 years

**Solutions**:
- Shield sensor wires from pump wires
- Apply conformal coating to connections
- Replace sensor if readings are permanently erratic
- Increase debounce requirement: `DECISION_DEBOUNCE_READINGS = 5`

### "Pump runs but no water delivered"

**Possible causes**:
1. **Airlock in tubing**: Bubble blocking flow
2. **Clogged pump**: Debris in impeller
3. **Kinked tubing**: Obstruction in line
4. **Empty reservoir**: Float switch failed

**Solutions**:
- Prime pump by submerging completely and tilting
- Remove pump, clean impeller chamber
- Straighten tubing, check for crush points
- Manually verify water level matches float reading

### "System enters ERROR state frequently"

**Possible causes**:
1. **DHT11 intermittent failure**: Loose connection
2. **Electrical noise**: Pump switching affects sensors
3. **Power brownout**: Insufficient current

**Solutions**:
- Resolder DHT11 connections
- Add 100nF capacitor between DHT11 VCC and GND
- Use separate power supplies for ESP32 and pump
- Verify power supply can deliver 500mA+ during pump operation

---

## Data Logging (Optional)

### Serial Log Capture

For troubleshooting, capture serial output to a file:

**Arduino IDE**: Serial Monitor → Enable timestamp → Copy/paste to file

**Python capture script** (runs on PC connected via USB):
```python
import serial
from datetime import datetime

ser = serial.Serial('COM3', 115200)  # Adjust port
with open('plant_log.txt', 'a') as f:
    while True:
        line = ser.readline().decode('utf-8').strip()
        timestamp = datetime.now().isoformat()
        f.write(f"{timestamp},{line}\n")
        print(f"{timestamp}: {line}")
```

### CSV Format for Analysis

Modify telemetry.h to output CSV-friendly format:

```cpp
// CSV header (print once at boot)
Serial.println("timestamp_ms,soil_pct,temp_c,humidity_pct,tank,pump_state,decision");

// Data line (print each cycle)
Serial.print(millis()); Serial.print(",");
Serial.print(data.soilMoisturePercent, 1); Serial.print(",");
Serial.print(data.temperatureC, 1); Serial.print(",");
Serial.print(data.humidityPercent, 1); Serial.print(",");
Serial.print(data.tankHasWater ? "1" : "0"); Serial.print(",");
Serial.print(controller.isRunning() ? "1" : "0"); Serial.print(",");
Serial.println(decision.reason);
```

---

## Success Metrics

Your deployment is successful when:

| Metric | Target | How to Verify |
|--------|--------|---------------|
| Uptime | >99% (1 reboot/week max) | Count unexpected restarts |
| Water cycles | 2-6 per day (varies by plant) | Count from logs |
| False positives | <1 per week | Watering when soil was wet |
| False negatives | 0 | Plant wilts despite system running |
| Plant health | Steady or improving | Visual inspection |
| Reservoir refills | Predictable (weekly) | Track refill frequency |

---

## Deployment Environments

### Indoor (Default)

- Stable temperature and humidity
- Consistent lighting
- Standard configuration works

### Greenhouse

**Adjustments needed**:
- Higher temp max: `TEMP_MAX_C = 45.0f`
- Higher humidity threshold: `HUMIDITY_MIN_PERCENT = 40.0f`
- Faster evaporation: consider shorter cooldown

### Balcony/Outdoor

**Adjustments needed**:
- Weatherproof enclosure (IP65+)
- Wider temp range: `TEMP_MIN_C = 5.0f`, `TEMP_MAX_C = 45.0f`
- Rain sensor (future enhancement)
- Freeze protection (disable watering below 5°C)

---

*Deployment Guide Version: 1.0*  
*Last Updated: April 2026*
