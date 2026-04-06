# Troubleshooting Guide

> **Quick fixes for common issues encountered during development and deployment.**

---

## Hardware Issues

### ESP32 Won't Boot or Upload

| Symptom | Cause | Solution |
|---------|-------|----------|
| Upload fails with "Connecting..." forever | Boot mode issue | Hold BOOT button while uploading |
| Constant reset loop | Strapping pin conflict | Disconnect GPIO0, 2, 12, 15 during boot |
| Brownout reset in serial | Insufficient power | Use USB 2.0 port or powered hub |
| No serial output | Wrong baud rate | Set to 115200 in Serial Monitor |

### DHT11 Sensor Problems

| Symptom | Cause | Solution |
|---------|-------|----------|
| NaN readings on boot | Cold sensor | Increase `delay(1500)` to `delay(2500)` in sensors.h |
| NaN readings intermittently | Electrical noise | Add 100nF capacitor between VCC and GND |
| Reads consistently 0°C/0% | Dead sensor | Replace sensor |
| Temperature offset (+5°C) | Sensor near heat source | Relocate sensor, add offset in code |
| Humidity always 99% | Condensation on sensor | Allow sensor to dry; avoid direct water spray |

### Soil Sensor Problems

| Symptom | Cause | Solution |
|---------|-------|----------|
| Always reads 0% | Sensor on 5V (damaged) | Reconnect to 3.3V, may need replacement |
| Always reads 100% | Probes not in medium | Insert probes into soil |
| Erratic readings | Corroded probes | Clean with fine sandpaper or replace |
| Readings drift slowly | Temperature effect | Normal; debounce filter handles this |
| Wildly jumping values | Loose connection | Check jumper wire connections |

### Float Switch Problems

| Symptom | Cause | Solution |
|---------|-------|----------|
| Always reads "Tank: available" | Wiring reversed | Swap the two float switch wires |
| Always reads "Tank: empty" | Float stuck in down position | Ensure free vertical movement |
| Status bounces rapidly | Water sloshing | Increase debounce: `50UL` → `200UL` in sensors.h |
| No change when tilted | Switch contacts corroded | Clean or replace switch |

### Relay/Pump Problems

| Symptom | Cause | Solution |
|---------|-------|----------|
| Relay won't click | Wrong VCC voltage | Connect relay VCC to VIN (5V), not 3.3V |
| Relay clicks but pump silent | Wrong terminal | Use NO terminal, not NC |
| Pump runs but no water | Air lock in tubing | Prime pump by submerging and tilting |
| Pump runs weak | Kinked tubing | Straighten tubing, check for obstructions |
| Pump runs indefinitely | Stuck relay | Watchdog should catch this; check code upload |

---

## Software Issues

### Decision Logic Problems

| Symptom | Cause | Solution |
|---------|-------|----------|
| Never waters despite dry soil | Debounce not satisfied | Wait 6 seconds (3 readings × 2 sec) |
| Waters when soil is wet | Calibration off | Recalibrate sensor (see CALIBRATION.md) |
| Skips watering in normal conditions | Environmental bounds | Check temp/humidity thresholds in config.h |
| "Tank is empty" when tank is full | Float switch wiring | Verify switch reads HIGH when water present |

### State Machine Problems

| Symptom | Cause | Solution |
|---------|-------|----------|
| Stuck in COOLDOWN | Cooldown timer running | Wait 60 seconds or reduce `PUMP_COOLDOWN_MS` |
| Stuck in ERROR | Sensor failures | Check DHT wiring; will auto-recover on stable readings |
| Pump starts but immediately stops | Multiple issues | Check serial output for specific reason |
| No state transitions | Loop not running | Verify `loop()` is being called; check for infinite loops |

### Timing Issues

| Symptom | Cause | Solution |
|---------|-------|----------|
| Watchdog triggers during normal operation | `PUMP_ON_DURATION_MS` > `PUMP_WATCHDOG_MS` | Ensure watchdog > 2× pump duration |
| Sensor reads too fast (NaN) | Polling faster than 2 sec | Check `SENSOR_READ_INTERVAL_MS` |
| System resets after ~50 days | millis() overflow | Should be handled; verify unsigned arithmetic |

---

## Serial Monitor Messages

### Understanding Telemetry Output

```
========== SYSTEM STATUS ==========
Uptime: 1234s                         ← System running for 1234 seconds
--- Sensor Snapshot ---
Soil: 45.2% [dry]                     ← Below 60% threshold, marked dry
Temp: 24.5 C                          ← Within safe range (15-40°C)
Humidity: 55.0 %                      ← Within safe range (>30%)
Tank: available                       ← Float switch detecting water

--- Decision ---
Action: HOLD                          ← Not watering (see reason below)
Reason: Waiting for stable dry reading ← Need 3 consecutive dry readings

Mode: monitoring                      ← Current state machine state
Pump: off                             ← Pump is not running
```

### Common Messages Explained

| Message | Meaning | Action |
|---------|---------|--------|
| "Soil moisture is fine" | Above threshold | Normal operation |
| "Waiting for stable dry reading" | Debounce accumulating | Wait for more readings |
| "Tank is empty" | Float switch open | Refill reservoir |
| "Temperature/humidity out of range" | Environmental block | Move to suitable location |
| "Cooldown is active" | Recently watered | Wait for cooldown to expire |
| "Ready to water" | All conditions passed | Pump will activate |
| "[event] Pump started" | Relay activated | Pump running for 5 seconds |
| "[event] Pump stopped" | Normal completion | Entering cooldown |
| "[warn] Sensor instability detected" | DHT errors | Check DHT wiring |
| "[info] Sensors recovered" | DHT working again | Returning to normal |

---

## Calibration Verification

### Quick Sanity Checks

1. **Sensor in air** should read **0-10%** moisture
2. **Sensor in water** should read **90-100%** moisture
3. **Room temperature** should read **within ±3°C** of actual
4. **Humidity** should be **within ±10%** of actual (DHT11 is not precise)
5. **Float switch** should toggle when moved **up/down**

### If Calibration Seems Wrong

1. Record air and water readings (raw ADC values from serial)
2. Update `map()` in sensors.h with your values
3. Re-upload and verify 0-100% range is correct
4. See CALIBRATION.md for detailed procedure

---

## Performance Issues

### High False Positive Rate (>10%)

**Causes:**
- Debounce too low
- Sensor picking up electrical noise
- Sensor in unstable soil position

**Solutions:**
1. Increase `DECISION_DEBOUNCE_READINGS` from 3 to 5
2. Add shielding to sensor wires (keep away from relay wires)
3. Reposition sensor in more stable soil location

### Sluggish Response (>30 seconds)

**Causes:**
- Debounce too high
- Cooldown too long
- Sensor read interval too long

**Solutions:**
1. Decrease `DECISION_DEBOUNCE_READINGS` to 2
2. Decrease `PUMP_COOLDOWN_MS` (carefully—protects pump)
3. Keep `SENSOR_READ_INTERVAL_MS` at 2000 (DHT requirement)

### Power Consumption Higher Than Expected

**Causes:**
- Pump running too often
- No sleep mode implemented
- USB serial keeping processor awake

**Solutions:**
1. Increase `SOIL_DRY_THRESHOLD_PERCENT` to trigger less often
2. Future enhancement: Add light sleep between sensor reads
3. Use external power supply instead of USB for long-term

---

## Field-Specific Issues

### Indoor Environment

| Issue | Solution |
|-------|----------|
| Sensor dries out too fast | Move away from heating vents |
| Low humidity triggering block | Lower `HUMIDITY_MIN_PERCENT` |
| Artificial light affecting readings | Normal; temperature effect is minimal |

### Outdoor/Balcony

| Issue | Solution |
|-------|----------|
| Rain saturating soil | Add rain sensor (future enhancement) |
| Direct sun heating sensor | Shade DHT11 from direct sunlight |
| Wind cooling DHT readings | Use shielded/housed sensor |
| Insects nesting in enclosure | Seal all openings, use mesh vents |

### Greenhouse

| Issue | Solution |
|-------|----------|
| Constant high humidity | Raise `HUMIDITY_MIN_PERCENT` or disable check |
| High temperature blocking | Increase `TEMP_MAX_C` to 45 or 50 |
| Condensation on electronics | Use conformal coating; improve ventilation |

---

## Recovery Procedures

### Full System Reset

1. Disconnect power
2. Wait 10 seconds
3. Reconnect power
4. Observe serial output for normal boot sequence

### Factory Reset (Clear State)

Upload code fresh:
1. In Arduino IDE, click Upload
2. All runtime state is reset (counters, timers, etc.)
3. Calibration values remain (they're in code)

### Emergency Pump Stop

If pump is stuck running and watchdog fails:
1. **Disconnect power immediately** (unplug USB)
2. Check relay for physical damage
3. Replace relay module if stuck closed
4. Re-upload code before reconnecting

---

## Getting Help

Before asking for help, gather:

1. **Serial monitor output** (full boot sequence + issue occurring)
2. **Configuration** (your config.h values)
3. **Hardware list** (exact components used)
4. **Wiring photo** (or description)
5. **Environment** (indoor/outdoor, temperature range)

This information helps diagnose issues quickly.

---

*Troubleshooting Guide Version: 1.0*  
*Last Updated: April 2026*
