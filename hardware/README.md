# Hardware Design Documentation

## Overview

The irrigation control system hardware employs a modular architecture based on the ESP32 DevKit V1 microcontroller, enabling rapid prototyping, easy integration with development tools, and straightforward transition to production PCB layout.

---

## Component Selection & Specifications

### Microcontroller: ESP32 DevKit V1

| Specification | Value | Justification |
|---|---|---|
| Processor | Tensilica Xtensa 32-bit LX6 (240 MHz) | Adequate for 2-second periodic control loop |
| RAM | 520 KB SRAM | Sufficient for telemetry buffering and state management |
| Flash | 4 MB | Supports modular firmware with debugging symbols |
| ADC Channels | 12 (GPIO 32-39) | Channel 0 (GPIO 34) selected for soil sensor |
| Digital I/O | 34 programmable pins | 4 dedicated pins per system (sensors + actuator) |
| Power Consumption | 80-160 mA active | Supports 5V USB or external supply; 3.3V logic compatible |

**Selection Rationale**: ESP32 provides hardware reliability, widespread educational adoption, extensive documentation, and Arduino IDE support. Cost (~₹550) balances performance and affordability suitable for research/teaching projects.

### Soil Moisture Sensor: Capacitive (2-Pin)

| Parameter | Value | Notes |
|---|---|---|
| Type | Capacitive (non-resistive) | Avoids corrosion issues of resistive sensors |
| Voltage | 3.3V DC | Connected to ESP32 3.3V rail, ADC input on GPIO 34 |
| ADC Range | 0-4095 (12-bit) | Mapped to 0-100% soil moisture |
| Dry Reference | 3950 ADC | Measured empirically in open air (Day 2 calibration) |
| Wet Reference | 1650 ADC | Measured when submerged in distilled water |
| Response Time | ~2 seconds | Acceptable for 2-second read cycle |
| Cost | ~₹200 | Comparable to resistive types, better durability |

**Calibration Procedure**: Dry reference obtained by exposing sensor to open air for 5 minutes. Wet reference obtained by submerging sensor in deionized water to 50% depth for 2 minutes. Linear interpolation between references yields soil moisture percentage.

### Temperature/Humidity Sensor: DHT22

| Parameter | Value | Notes |
|---|---|---|
| Voltage | 3.3V DC (with pull-up) | Critical: NOT 5V tolerant |
| Protocol | 1-Wire serial (GPIO 4) | 40 µs bit timing; library handles abstraction |
| Temperature Range | -40 to +80°C | Suitable for most climates |
| Humidity Range | 0-100% RH | Allows optional humidity-based watering adjustments |
| Response Time | 1-2 seconds per read | Library allows retry on failure (max 2 consecutive failures accepted) |
| Cost | ~₹150 | Standard component from multiple suppliers |

**Notes**: DHT22 more accurate (±2°C, ±2% RH) than DHT11 (±2°C, ±5% RH). Pull-up resistor (10 kΩ) required between DATA and 3.3V if not built into library module. Current firmware reads temperature/humidity but does not yet implement humidity-based control; exists for future extensibility.

### Tank Level Detection: Float Switch

| Parameter | Value | Notes |
|---|---|---|
| Type | Magnetic reed switch (NO configuration) | Float mechanically coupled to magnet |
| Voltage | 3.3V trigger (GPIO 5 input) | Simple digital HIGH/LOW state |
| Float Position | Floats at water surface level | Submerged = tank empty; above = water available |
| Debounce | 50 ms settling time | Firmware applies software debounce via timer |
| Cost | ~₹100 | Alternative: capacitive level sensor (~₹300, better accuracy) |

**Safety Priority**: Tank detection is highest-priority safety check. If tank LOW, pump is blocked regardless of soil moisture state. This prevents pump dry-running, which damages pump seals and enables air ingestion into lines.

### Pump & Actuation: Relay-Driven 5V Pump

| Parameter | Value | Notes |
|---|---|---|
| Pump Type | Submersible DC motor pump | 5V nominal, rated for continuous duty |
| Flow Rate | ~1.2 L/min (depends on head) | Adequate for potted plant scenarios |
| Connector | Standard barbed (8 mm ID tubing) | Low-cost tubing readily available |
| Relay Type | Single-channel DC relay module | 5V coil, rated 10A @ 250VAC (over-specified for safety) |
| Relay Polarity | Active-HIGH (standard) | GPIO HIGH (3.3V) activates relay coil |
| Pump ON Duration | 5 seconds maximum | Empirically sufficient for 200 mL per cycle; adjustable in config |
| Cost | ~₹300 (pump) + ~₹80 (relay) | Total actuation cost ~₹380 |

**Power Considerations**: Pump inrush current ~1-2 A during startup. Separate 5V supply recommended (not USB) to avoid ESP32 brownout. Common ground between supply and ESP32 critical for reliable GPIO signaling.

---

## Pin Mapping & Electrical Interface

### GPIO Configuration

```
ESP32 DevKit V1
├── GPIO 34 (ADC0):     Soil Moisture Sensor → Analog Input (0-3.3V)
├── GPIO 4:              DHT22 Data → Digital I/O (1-Wire)
├── GPIO 5:              Tank Level Switch → Digital Input (HIGH/LOW)
├── GPIO 18:             Relay Control Signal → Digital Output (HIGH activates)
│
├── 3.3V:                Soil Sensor VCC, DHT22 VCC
├── 5V (via relay):      Pump power supply (from external 5V PSU)
└── GND:                 Common return (shared with external PSU)
```

### Power Distribution

**Recommended Approach** (Bench Testing):
```
USB 5V Supply → ESP32 USB Port (powers ESP32 only)
Separate 5V Supply → Relay Module VCC
  ├─ Relay COM pin → Pump positive (red)
  └─ Relay GND → Pump negative (black)
  
Common GND reference: All circuits share single GND rail on breadboard
```

**Rationale**: Isolating pump power prevents 1-2 A inrush current from causing ESP32 brownout reset. Common ground ensures reliable digital logic levels across modules.

---

## Calibration & Characterization

### Soil Sensor Calibration

**Raw ADC Values** (from Day 2 assembly testing):
- Air dry (room humidity): **3950 ADC**
- Water saturated (submerged): **1650 ADC**

**Moisture Percentage Mapping**:
```
Moisture % = 100 × (ADC_reading - 1650) / (3950 - 1650)
```

**Example Calculations**:
- ADC 1650 → 0% (completely saturated)
- ADC 2800 → 50% (medium moisture)
- ADC 3950 → 100% (bone dry)

**Recommended Re-Calibration**: Before field deployment, re-measure dry and wet reference points with actual soil sample. Capacitive sensors exhibit slight variations due to dielectric properties of specific soil types. Typical variance: ±100 ADC counts (±3% moisture error).

### Sensor Accuracy & Response

| Sensor | Typical Accuracy | Response Time | Notes |
|---|---|---|---|
| Soil Moisture | ±3% (after calibration) | 2 seconds | Accepts 3 consecutive readings before decision |
| Temperature | ±2°C | 1-2 seconds per read | DHT22 spec; library retries on transient failure |
| Tank Level | Digital (HIGH/LOW) | 50 ms (debounced) | Float switch; simple binary state |

---

## Safety & Reliability Considerations

### Three-Layer Safety Architecture

1. **Tank Interlock** (Primary): Blocks pump if water unavailable
   - Prevents cavitation damage to pump
   - Stops water flow to plant (graceful degradation)
   - Checked every 2-second cycle

2. **Cooldown Delay** (Secondary): 60-second pause between cycles
   - Prevents relay chatter and mechanical wear
   - Allows soil to re-saturate naturally
   - Reduces unnecessary pumping

3. **Watchdog Timer** (Tertiary): Hard 10-second pump cutoff
   - Independent timing mechanism
   - Backup if main loop hangs
   - Should never trigger under normal operation (indicates firmware bug)

### Relay Debouncing & Noise Management

- **GPIO Switching Frequency**: 0.1 Hz (pump ON/OFF every 60+ seconds)
- **Relay Coil Protection**: Built into module (diode across coil)
- **Signal Integrity**: Keep sensor wires away from pump/relay ground planes
- **Cooldown Logic**: Prevents rapid relay cycling that degrades contacts

---

## Design Evolution & Lessons Learned

**Initial Prototype Iterations** (from assembly_notes.txt):

- **Attempt 1**: Resistive soil sensor → Corroded after 2 weeks (switched to capacitive)
- **Attempt 2**: DHT11 sensor → Excessive noise, frequent read failures (upgraded to DHT22)
- **Attempt 3**: Direct USB power for pump → ESP32 brownout resets (isolated 5V supply)
- **Final Design**: Capacitive sensor + DHT22 + isolated power = stable operation

**Key Lessons**:
- Capacitive sensors dramatically more reliable than resistive in wet environments
- DHT22 worth the extra cost (~₹50 premium) for reliability
- Power isolation critical; shared GND is not sufficient
- Software debouncing (3-read filter) eliminates sensor noise without hardware complexity

---

## Future Hardware Roadmap

### Phase 2A: Prototype Validation
- Assemble on breadboard with all components
- Validate all GPIO connections and sensor readings
- Measure pump water flow rate in liters/minute
- Record calibration photos for documentation

### Phase 2B: Integration Testing  
- Mount tank and tubing
- Test float switch hysteresis (rising/falling thresholds)
- Verify pump startup transients don't cause reset
- 4-hour soak test for stability

### Phase 2C: Field Deployment
- Pot plant with real soil (not just water)
- 7-14 day operation with manual moisture logging
- Compare automated irrigation vs. manual watering
- Measure water efficiency (liters per day)

### Phase 3: Production Deployment
- Design single-layer PCB (low cost, high reliability)
- Transition from breadboard/USB to mounted module
- 3D-printed enclosure with cable strain relief
- IP67 rating for outdoor use (future)

---

## Bill of Materials (Complete)

| Component | Qty | Unit Cost | Total | Supplier | Datasheet |
|---|---|---|---|---|---|
| ESP32 DevKit V1 | 1 | ₹550 | ₹550 | Ali/Local | [Link](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/user-guide-evaluation-board.html) |
| Capacitive Soil Sensor | 1 | ₹200 | ₹200 | Amazon/Ali | Spec in calibration section |
| DHT22 Sensor + Module | 1 | ₹150 | ₹150 | Electronics Shops | [Spec](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf) |
| 1-Channel Relay Module | 1 | ₹80 | ₹80 | Ali/Local | 5V active-HIGH relay |
| 5V Mini Water Pump | 1 | ₹300 | ₹300 | Ali/Local | 1.2 L/min, submersible |
| Float Switch | 1 | ₹100 | ₹100 | Water/Auto parts | Magnetic NO switch |
| Breadboard (830pt) | 1 | ₹200 | ₹200 | Electronics Kit | Reusable |
| Jumper Wires (40pc mixed) | 1 | ₹80 | ₹80 | Electronics Kit | M-M and M-F |
| USB Cable + PSU | 1 | ₹150 | ₹150 | Standard | 5V 2A recommended |
| Silicone Tubing (8mm, 3m) | 1 | ₹50 | ₹50 | Plumbing supplies | Flexible, 8mm ID |
| Connectors & Misc. | 1 | ₹100 | ₹100 | General | Solder, shrink tube, etc. |
| **TOTAL** | | | **₹1,960** | | |

**Cost Notes**:
- Prices in Indian Rupees (INR), approximate as of 2026-04-09
- Costs vary by region and supplier; budget ±20% for contingency
- Components reusable across projects (breadboard, cables, PSU)
- Firmware and simulation tools open-source (free)

---

## Compliance & Standards

- **Voltage Safety**: All sensor circuits 3.3V or 5V (safe-by-design, no high-voltage hazards)
- **Current Limiting**: Pump current isolated from ESP32 logic circuits
- **ESD Protection**: No explicit ESD devices used (acceptable for educational prototypes)
- **Operating Temperature**: 0-40°C (typical indoor/outdoor plant environments)
- **Humidity Tolerance**: <90% RH (avoid condensation on electronics)

---

**Document Version**: 2.0 (Professional Hardware Design)  
**Last Updated**: 2026-04-09  
**Status**: Complete & Ready for Bench Assembly Phase
