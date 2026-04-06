# Sensor Calibration Guide

> **Purpose**: Ensure accurate readings by calibrating sensors to your specific hardware and environment.

---

## Why Calibration Matters

Factory sensors ship with generic parameters. Your actual readings depend on:

- **Soil sensor**: Probe condition, soil type, insertion depth
- **DHT11**: Ambient temperature, age, batch variation
- **Float switch**: Mounting position, water density

Spending 15 minutes on calibration dramatically improves decision accuracy.

---

## Soil Moisture Sensor Calibration

### Understanding the Sensor Output

Capacitive soil sensors measure dielectric permittivity:
- **Dry air**: Low permittivity → High resistance → High ADC value (~4095)
- **Water**: High permittivity → Low resistance → Low ADC value (~1200-2000)
- **Moist soil**: Somewhere between, depending on water content

The default code assumes:
```cpp
// sensors.h - default mapping
map(rawSoil, 4095, 0, 0, 100)
// This means: 4095 → 0%, 0 → 100%
```

This is rarely accurate. Calibration finds YOUR sensor's actual range.

### Calibration Procedure (15 min)

**Equipment needed**: 
- Glass of water
- Dry towel
- Serial monitor open (115200 baud)

#### Step 1: Air Reading (Dry Baseline)

1. Hold sensor in open air, probes pointing down
2. Wait 5 seconds for reading to stabilize
3. Record the raw ADC value from serial monitor
4. Typical range: **3800-4095**

```
Your air reading: _________ (record this)
```

#### Step 2: Water Reading (Wet Baseline)

1. Submerge probes (NOT the PCB) in water
2. Keep PCB above water surface
3. Wait 10 seconds (water takes longer to stabilize)
4. Record the raw ADC value
5. Typical range: **1200-2000**

```
Your water reading: _________ (record this)
```

#### Step 3: Update Code

Open `include/sensors.h` and find this line (~line 42):

```cpp
// BEFORE (default)
data.soilMoisturePercent = static_cast<float>(map(rawSoil, 4095, 0, 0, 100));

// AFTER (calibrated - replace with YOUR values)
data.soilMoisturePercent = static_cast<float>(map(rawSoil, YOUR_AIR_VALUE, YOUR_WATER_VALUE, 0, 100));
```

**Example**: If your air = 3950, water = 1650:
```cpp
data.soilMoisturePercent = static_cast<float>(map(rawSoil, 3950, 1650, 0, 100));
```

#### Step 4: Validation

After uploading calibrated code:

| Test | Expected Reading | Action if Wrong |
|------|------------------|-----------------|
| Sensor in air | 0-10% | Increase air value |
| Sensor in water | 90-100% | Decrease water value |
| Dry potting soil | 10-30% | Should be in range |
| Wet soil after watering | 70-90% | Should be in range |

### Soil Type Adjustment

Different soils hold water differently. Adjust `SOIL_DRY_THRESHOLD_PERCENT` based on your medium:

| Soil Type | Dry Threshold | Rationale |
|-----------|---------------|-----------|
| Sandy/cactus mix | 70% | Drains fast, water early |
| Loamy/general potting | 60% | Balanced retention |
| Clay/peat-heavy | 50% | Retains water, delay watering |
| Hydroton/LECA | 80% | Very fast drainage |

### Calibration Curve (Optional Advanced)

For maximum accuracy, create a multi-point calibration:

```
ADC Value | Actual Moisture | Notes
----------|-----------------|------
3900      | 0%              | Bone dry
3200      | 25%             | Slightly moist
2500      | 50%             | Moderately wet
1800      | 75%             | Quite wet
1500      | 100%            | Saturated
```

Then implement piecewise linear interpolation instead of simple map().

---

## DHT11 Temperature/Humidity Validation

### Understanding DHT11 Limitations

| Specification | DHT11 | DHT22 (upgrade) |
|---------------|-------|-----------------|
| Temperature accuracy | ±2°C | ±0.5°C |
| Humidity accuracy | ±5% RH | ±2% RH |
| Sampling rate | 1 Hz max | 0.5 Hz |
| Operating range | 0-50°C, 20-90% RH | -40-80°C, 0-100% RH |

DHT11 is adequate for safety bounds checking (not precision horticulture).

### Validation Against Reference (10 min)

**Equipment**: Known-accurate thermometer or weather station

1. Place DHT11 and reference thermometer side-by-side
2. Wait 10 minutes for thermal equilibrium
3. Record both readings
4. Calculate offset: `offset = DHT11_reading - Reference_reading`

**If offset is significant** (>3°C), add software compensation:

```cpp
// In sensors.h, after reading temperature:
data.temperatureC = dht_.readTemperature();
data.temperatureC -= 2.5f;  // Example: DHT11 reads 2.5°C high
```

### Common DHT11 Issues

| Issue | Symptom | Solution |
|-------|---------|----------|
| NaN readings | Serial shows "nan" | Add longer delay after begin() |
| Stuck value | Same reading for hours | Check wiring, try different GPIO |
| Offset error | Reads consistently high/low | Apply software compensation |
| Slow response | Takes minutes to track temp change | Normal for DHT11, wait longer |

### Warm-Up Period

DHT11 needs time to stabilize after power-on:

```cpp
// In sensors.h begin()
dht_.begin();
delay(1500);  // Current: 1.5 seconds
// Increase to delay(2500) if seeing NaN on first readings
```

---

## Pump Flow Rate Measurement

### Why Measure Flow Rate

The `PUMP_ON_DURATION_MS` setting determines water volume delivered:

```
Volume = Flow Rate × Time
```

Knowing your actual flow rate allows precise watering calculations.

### Measurement Procedure (5 min)

**Equipment**: Measuring cup or graduated cylinder, stopwatch (phone works)

1. Fill reservoir with water
2. Start stopwatch and pump simultaneously
3. Run pump for exactly **10 seconds**
4. Stop pump and measure water collected
5. Calculate: `Flow rate = Volume ÷ 10 seconds`

**Example**:
- Collected: 280 ml in 10 seconds
- Flow rate: 28 ml/sec

### Adjusting Pump Duration

**Target watering volume** depends on pot size:

| Pot Diameter | Estimated Root Volume | Target Water/Cycle |
|--------------|----------------------|-------------------|
| 8 cm | 300 ml | 100 ml |
| 12 cm | 1000 ml | 150 ml |
| 15 cm | 2000 ml | 250 ml |
| 20 cm | 4000 ml | 400 ml |

**Calculate duration**:
```
Duration = Target Volume ÷ Flow Rate

Example: 150 ml ÷ 28 ml/sec = 5.4 seconds ≈ 5500 ms
```

Update `config.h`:
```cpp
constexpr unsigned long PUMP_ON_DURATION_MS = 5500UL;  // Adjusted for your pump
```

### Flow Rate Degradation

Over time, pumps lose efficiency due to:
- Mineral deposits
- Tubing kinks
- Impeller wear

**Recommendation**: Re-measure flow rate monthly and adjust if >10% deviation.

---

## Float Switch Calibration

### Mounting Position

The float switch triggers when water level drops below its position:

```
┌─────────────┐
│             │ ← Mount switch 2-3 cm above pump intake
│  ──FLOAT──  │    This ensures ~100ml reserve when switch triggers
│             │
│  ╔═══════╗  │
│  ║ PUMP  ║  │ ← Pump intake at bottom
│  ╚═══════╝  │
└─────────────┘
```

### Verifying Switch Behavior

| Water Level | Float Position | GPIO5 Reading | System Response |
|-------------|----------------|---------------|-----------------|
| Above switch | Float up (closed) | HIGH | "Tank: available" |
| Below switch | Float down (open) | LOW | "Tank: empty" |

**Test procedure**:
1. Fill reservoir above float switch
2. Verify serial shows "Tank: available"
3. Drain below float switch
4. Verify serial shows "Tank: empty"
5. Verify pump does NOT activate when soil is dry but tank is empty

### Debounce Consideration

Float switches can bounce (rapid on/off) when water is sloshing:

```cpp
// In sensors.h - current debounce: 50ms
if (millis() - tankDebounceStart_ > 50UL) {
```

If experiencing false triggers, increase to 200ms:
```cpp
if (millis() - tankDebounceStart_ > 200UL) {
```

---

## Environmental Threshold Tuning

### Default Safety Bounds

```cpp
// config.h
constexpr float TEMP_MIN_C = 15.0f;        // Below: cold stress risk
constexpr float TEMP_MAX_C = 40.0f;        // Above: heat stress risk
constexpr float HUMIDITY_MIN_PERCENT = 30.0f;  // Below: dry stress
```

### Plant-Specific Adjustments

| Plant Type | Temp Min | Temp Max | Humidity Min | Notes |
|------------|----------|----------|--------------|-------|
| Tropical (Monstera, Pothos) | 18°C | 35°C | 40% | Prefer higher humidity |
| Succulents/Cacti | 10°C | 45°C | 20% | Tolerate extremes |
| Herbs (Basil, Mint) | 15°C | 30°C | 35% | Moderate conditions |
| Vegetables | 12°C | 35°C | 30% | Varies by species |

### Seasonal Adjustment

Consider creating seasonal profiles:

```cpp
// Summer profile (higher tolerance for heat)
constexpr float TEMP_MAX_C = 42.0f;

// Winter profile (protect from cold, indoor heating = low humidity OK)
constexpr float TEMP_MIN_C = 12.0f;
constexpr float HUMIDITY_MIN_PERCENT = 25.0f;
```

---

## Calibration Log Template

Keep a calibration record for troubleshooting:

```
CALIBRATION LOG
===============

Date: ____________

SOIL SENSOR
  Model: ____________________
  Air reading: _______ ADC
  Water reading: _______ ADC
  Dry threshold set: _______ %
  
DHT11
  Reference temp: _______ °C
  DHT11 reading: _______ °C  
  Offset applied: _______ °C
  
PUMP
  Model: ____________________
  Flow rate measured: _______ ml/sec
  Duration set: _______ ms
  Target volume: _______ ml
  
FLOAT SWITCH
  Type: ____________________
  Debounce set: _______ ms
  Trigger height: _______ cm above pump

NOTES:
________________________________
________________________________
________________________________
```

---

## Verification Checklist

After calibration, verify the complete system:

- [ ] Soil sensor reads 0-10% in air, 90-100% in water
- [ ] Dry potting soil reads 20-40%
- [ ] Wet soil after watering reads 70-90%
- [ ] Temperature within ±3°C of reference
- [ ] Humidity reasonable for environment
- [ ] Tank empty correctly stops pump
- [ ] Pump delivers expected volume in expected time
- [ ] System completes full watering cycle without errors
- [ ] Cooldown timer functions correctly

---

*Calibration Guide Version: 1.0*  
*Last Updated: April 2026*
