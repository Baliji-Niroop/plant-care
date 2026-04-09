# Hardware Build Guide: Stage 1 - Bench Prototype

## Overview
This guide walks through assembling the irrigation controller on a breadboard for testing and validation before final enclosure.

**Estimated Time**: 1-2 hours (including wiring verification)
**Required**: All components from `components_list.txt`

---

## Stage 1: Bench Wiring Setup

### Prerequisites
- [ ] All components sourced (see `components_list.txt`)
- [ ] ESP32 DevKit V1 flashed with latest firmware (`firmware/main.ino`)
- [ ] USB cable for ESP32 serial monitoring
- [ ] Breadboard (830+ point recommended)
- [ ] Jumper wires (mixed length)
- [ ] 5V power supply (separate from ESP32 USB)

### Wiring Checklist

#### 1. Power Distribution (5V Supply Line)
```
5V Supply Positive → Breadboard Red Rail
5V Supply Negative → Breadboard Blue Rail (Ground)
ESP32 GND (multiple pins) → Breadboard Blue Rail
ESP32 5V or VIN → Breadboard Red Rail (if available; otherwise use USB)
```

**Note**: Ensure common ground between ESP32 and external 5V supply to prevent floating reference.

---

#### 2. Soil Moisture Sensor (Capacitive)

| Component Pin | ESP32 GPIO | Notes |
|---|---|---|
| Sensor VCC | 3.3V (or 5V if rail voltage) | Use 3.3V for ADC safety |
| Sensor GND | GND | Common ground |
| Sensor AO (Analog Out) | GPIO 34 (ADC0) | Analog input, no pull-up needed |
| Sensor DO (Digital Out) | NC (unused) | Leave unconnected |

**Verification**:
- [ ] Sensor reads ~3950 ADC in dry air
- [ ] Sensor reads ~1650 ADC when submerged in water
- [ ] Reading stabilizes within 2 seconds of insertion

---

#### 3. DHT22 Sensor (Temperature & Humidity)

| Component Pin | ESP32 GPIO | Notes |
|---|---|---|
| DHT VCC | 3.3V | **Critical: 3.3V only, not 5V** |
| DHT GND | GND | Common ground |
| DHT Data | GPIO 4 | Requires pull-up (built into library) |
| DHT NC | NC | Leave unconnected |

**Verification**:
- [ ] Serial output shows temperature and humidity readings
- [ ] Readings update every 2 seconds (sensor read interval)
- [ ] No "DHT read failed" messages on startup (1-2 failures acceptable on power-on)

---

#### 4. Tank Level Switch (Digital)

| Component Pin | ESP32 GPIO | Notes |
|---|---|---|
| Switch VCC | 3.3V or 5V | Use same rail as relay logic |
| Switch GND | GND | Common ground |
| Switch Signal | GPIO 5 | Digital input, HIGH when water present |

**Wiring Notes**:
- Float switch: wire to NO (Normally Open) pin if using multi-pin connector
- Position float so it naturally floats at desired level
- Use ~50ms debounce in firmware (already configured)

**Verification**:
- [ ] Serial output shows `tank_available: 1` when float rises (water high)
- [ ] Serial output shows `tank_available: 0` when float falls (water low)
- [ ] No jitter (stable reading without oscillation)

---

#### 5. Relay Module (1-Channel)

| Component Pin | ESP32 GPIO | Notes |
|---|---|---|
| Relay VCC | 5V | **Must use 5V rail** |
| Relay GND | GND | Common ground |
| Relay IN (Signal) | GPIO 18 | Control pin, active HIGH to turn on |
| Relay COM | (pump input) | Middle pin (common) |
| Relay NO | (pump outlet) | Normally Open - pump wired here |

**Wiring Notes**:
- **Active-High**: HIGH (3.3V from ESP32) turns relay on
- Verify relay type (some are active-low; this guide assumes active-high)
- COM connects to positive from pump power supply
- Pump negative connects to power supply negative
- Ensure pump power supply is isolated from ESP32 USB power

**Verification**:
- [ ] Relay clicking audibly when GPIO 18 goes HIGH
- [ ] LED on relay module lights when relay engages
- [ ] Relay does NOT engage with GPIO 18 LOW (pump off)

---

#### 6. Pump (5V Mini Water Pump)

| Component | Connection | Notes |
|---|---|---|
| Pump Positive (red) | Relay NO pin | Switched by relay |
| Pump Negative (black) | 5V Supply GND | Common return path |
| Water Input | Tank intake line | Use tubing (typically 8mm ID) |
| Water Output | Plant pot or test container | Controlled outlet |

**Safety Verification**:
- [ ] Pump does NOT run when powered (relay off state)
- [ ] Pump ONLY runs when relay relay engages (controlled by GPIO 18)
- [ ] No water leaks at tubing connections
- [ ] Pump test run ≤ 5 seconds to confirm operation (hardware PUMP_ON_DURATION_MS = 5000)

---

### Breadboard Layout Example
```
┌─────────────────────────────────────────┐
│ ESP32 DevKit V1  (USB left, pins right) │
│ GPIO 34 ----→ Soil Sensor AO           │
│ GPIO 4  ----→ DHT22 Data               │
│ GPIO 5  ----→ Tank Switch Signal       │
│ GPIO 18 ----→ Relay IN (Signal)        │
│ 3.3V    ----→ DHT, Soil VCC (3.3V)    │
│ 5V/VIN  ----→ Relay VCC                │
│ GND     ----→ GND Rail (common)        │
└─────────────────────────────────────────┘
       ↓
   [Breadboard with power rails]
       ↓
    GND ←────→ Tank Switch, Relay, Pump GND
    5V  ←────→ Relay VCC, Pump + via relay
```

---

## Stage 1: Validation Checklist

### Serial Output Verification
1. **Open Serial Monitor** (Arduino IDE or VS Code Pylance)
   - Baud rate: `115200`
   - Expected first line: System initialization message
   
2. **Check Sensor Readings** (should see every 3 seconds):
   ```
   [12:34:56] Temp: 25.3°C, Humidity: 45%, Soil: 2100 ADC (42%), Tank: HIGH, Pump: OFF
   [12:34:59] Temp: 25.2°C, Humidity: 45%, Soil: 2050 ADC (40%), Tank: HIGH, Pump: OFF
   ...
   ```
   
3. **Verify All Fields Present**:
   - ✅ Timestamp
   - ✅ Temperature & Humidity
   - ✅ Soil ADC + percentage
   - ✅ Tank status
   - ✅ Pump state
   - ✅ Decision reason

### Behavior Verification
1. **Dry Soil Trigger**:
   - Place soil sensor in dry air or partially in dry soil
   - Wait 3+ seconds for readings to drop
   - Expected: Soil % drops below 60%, pump should activate within 6 seconds
   - Observe pump engaged and relay clicking

2. **Wet Soil Hold**:
   - Return soil sensor to water or humid environment
   - Expected: Soil % rises, pump stops within 10 seconds (watchdog max)
   - Observe relay releases

3. **Tank Empty Override**:
   - Push down float switch (tank low)
   - Even with dry soil, pump should NOT engage
   - Expected: Serial shows `tank_available: 0, pump: OFF, decision: hold_tank_empty`

4. **Cooldown Enforcement**:
   - Trigger one watering cycle (dry soil)
   - Immediately repeat dry soil trigger
   - Expected: Pump does NOT activate; instead shows cooldown timer (60 seconds)

---

## Troubleshooting

| Issue | Likely Cause | Fix |
|-------|---|---|
| No serial output | USB driver missing or wrong baud rate | Update CH340 driver, verify 115200 baud |
| Soil sensor reads 0 or 4095 constantly | GPIO 34 not wired or ADC issue | Verify GPIO 34 connection, try jumper wiggle |
| DHT22 "read failed" every cycle | Power too low or loose connection | Check 3.3V rail, re-seat sensor |
| Tank switch always HIGH | Float stuck or not wired | Check float movement, verify GPIO 5 connection |
| Relay always on | GPIO 18 pulled HIGH unintentionally | Check for short, verify firmware config |
| Pump never activates | Relay not engaging or polarity wrong | Test relay with direct 5V; check pump polarity |
| Pump stays on indefinitely | Watchdog not triggering | Verify watchdog enabled in config; check GPIO 18 |

---

## Next: Photography & Documentation

Once all systems verified:

1. **Take overhead photo** of breadboard layout showing all connections
2. **Take side photo** with pump action in progress (for evidence)
3. **Save as**: `docs/images/prototype-stage1-placeholder.png` (replace placeholder)
4. **Record**: Serial output screenshot showing all four validation scenarios
5. **Update**: `hardware/README.md` with "Stage 1: ✅ Complete" and photo date

---

## After Stage 1: Next Steps

- [ ] Verify all calibration values match firmware config
- [ ] Plan Stage 2 enclosure design and mounting
- [ ] Begin component isolation: consider moving to proto board or PCB
- [ ] Extended soak test: run for 24 hours and monitor for drift
