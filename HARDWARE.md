# Hardware Assembly Guide

> **Build Time**: ~45 minutes (first-time) | ~20 minutes (experienced)  
> **Total Cost**: ₹1,350 (~$16 USD)  
> **Difficulty**: Beginner-friendly

---

## Bill of Materials

| # | Component | Specification | Qty | Unit Cost (₹) | Source | Notes |
|---|-----------|---------------|-----|---------------|--------|-------|
| 1 | ESP32 DevKit | 38-pin, CP2102 USB | 1 | 450 | [Robu.in](https://robu.in) | Prefer CP2102 chip (stable drivers) |
| 2 | Soil Moisture Sensor | Capacitive v1.2/v2.0 | 1 | 150 | Amazon.in | **Avoid resistive type** (corrodes) |
| 3 | DHT11 Module | 3-pin breakout | 1 | 80 | Amazon.in | Upgrade path: DHT22 (±₹120 more) |
| 4 | Relay Module | 1-channel, 5V coil | 1 | 70 | Amazon.in | Verify: 5V logic compatible |
| 5 | Submersible Pump | 3-6V DC, mini | 1 | 200 | Amazon.in | Flow: 80-120 L/hr typical |
| 6 | Float Switch | Vertical mount | 1 | 100 | Amazon.in | Or: magnetic reed switch |
| 7 | Silicone Tubing | 6mm ID, 1 meter | 1 | 50 | Hardware store | Food-grade preferred |
| 8 | Jumper Wires | M-M and M-F, 20 pcs | 1 | 50 | Amazon.in | Use color coding! |
| 9 | Breadboard | 830 tie-points | 1 | 100 | Amazon.in | Half-size OK for compact build |
| 10 | Power Supply | 5V 2A, micro-USB | 1 | 150 | Amazon.in | Or use USB power bank |
| | **TOTAL** | | | **₹1,400** | | Budget-friendly |

### Optional Upgrades

| Component | Cost | Benefit |
|-----------|------|---------|
| DHT22 instead of DHT11 | +₹120 | ±0.5°C vs ±2°C accuracy |
| PCB prototype board | +₹60 | Permanent installation (no breadboard) |
| IP65 enclosure | +₹200 | Outdoor/greenhouse deployment |
| 18650 battery + TP4056 | +₹300 | Portable/solar-ready |

---

## Pin Mapping Reference

```
                    ┌───────────────────┐
                    │    ESP32 DevKit   │
                    │                   │
                3V3 │●                 ●│ VIN (5V in)
                GND │●                 ●│ GND
               GPIO15│●                 ●│ GPIO13
               GPIO2 │●                 ●│ GPIO12
        DHT11 → GPIO4│●  ════════════  ●│ GPIO14
               GPIO16│●  │          │  ●│ GPIO27
               GPIO17│●  │  ESP32   │  ●│ GPIO26
        RELAY → GPIO18│●  │  CHIP    │  ●│ GPIO25
               GPIO19│●  │          │  ●│ GPIO33
               GPIO21│●  ════════════  ●│ GPIO32
               GPIO22│●                 ●│ GPIO35
               GPIO23│●                 ●│ SOIL → GPIO34
                 GND │●                 ●│ GPIO39
     TANK SW → GPIO5 │●                 ●│ GPIO36
                    └───────────────────┘
```

| Component | GPIO | Direction | Notes |
|-----------|------|-----------|-------|
| Soil Moisture Sensor | GPIO34 | INPUT (analog) | ADC1_CH6, no WiFi conflict |
| DHT11 Temperature/Humidity | GPIO4 | INPUT/OUTPUT | Bidirectional 1-wire |
| Tank Float Switch | GPIO5 | INPUT_PULLUP | Active HIGH = water present |
| Relay Control | GPIO18 | OUTPUT | HIGH = relay closed = pump ON |

---

## Wiring Diagram

```
                                          ┌─────────────┐
  ┌──────────────────────────────────────│   ESP32     │──────────────────────────────────────┐
  │                                       │   DevKit    │                                      │
  │                                       └──────┬──────┘                                      │
  │                                              │                                             │
  │    ┌───────────────┬─────────────────┬──────┴─────┬─────────────────┬───────────────┐    │
  │    │               │                 │            │                 │               │    │
  │    │               │                 │            │                 │               │    │
  │    ▼               ▼                 ▼            ▼                 ▼               ▼    │
  │ ┌──────┐      ┌─────────┐      ┌─────────┐   ┌────────┐      ┌───────────┐    ┌─────┐   │
  │ │ 3.3V │      │  GND    │      │  GPIO4  │   │ GPIO5  │      │  GPIO18   │    │GPIO34│  │
  │ └──┬───┘      └────┬────┘      └────┬────┘   └───┬────┘      └─────┬─────┘    └──┬───┘  │
  │    │               │                │            │                 │             │      │
  │    │               │                │            │                 │             │      │
  │    │  ┌────────────┴───────┐        │            │                 │             │      │
  │    │  │                    │        │            │                 │             │      │
  │    │  │    ┌───────────────┼────────┴────┐       │                 │             │      │
  │    │  │    │               │             │       │                 │             │      │
  │    │  │    │               │             │       │                 │             │      │
  │    ▼  ▼    ▼               ▼             │       ▼                 ▼             ▼      │
  │  ┌─────────────┐     ┌──────────┐        │   ┌──────────┐    ┌──────────┐  ┌─────────┐ │
  │  │   DHT11     │     │  SOIL    │        │   │  FLOAT   │    │  RELAY   │  │  SOIL   │ │
  │  │   Module    │     │  SENSOR  │        │   │  SWITCH  │    │  MODULE  │  │ SENSOR  │ │
  │  │             │     │          │        │   │          │    │          │  │  (SIG)  │ │
  │  │ VCC DAT GND │     │ VCC GND  │        │   │ NO  COM  │    │ VCC IN GND│  │         │ │
  │  └─┬───┬───┬───┘     └─┬───┬────┘        │   └─┬────┬───┘    └─┬──┬──┬──┘  └────┬────┘ │
  │    │   │   │           │   │             │     │    │          │  │  │          │      │
  │    │   │   │           │   │             │     │    │          │  │  │          │      │
  │    │   │   │           │   │             │     │    │          │  │  │          │      │
  │    3.3V│  GND         3.3V GND           └─────┘    │         5V  │ GND         │      │
  │        │                                       3.3V(via        │              3.3V     │
  │        │                                       pullup)         │                       │
  │        │                                                       │                       │
  │        └──────────────────────────────────────────────────────►│◄──────────────────────┘
  │                              GPIO4 (data)                      │
  │                                                                │
  │                                                          ┌─────┴─────┐
  │                                                          │   PUMP    │
  │                                                          │  (via NO  │
  │                                                          │  terminal)│
  │                                                          └───────────┘
  │
  └──────────────────────────────────────────────────────────────────────────────────────────
```

### Connection Details

#### DHT11 Temperature & Humidity Sensor
```
DHT11 Module          ESP32
─────────────         ─────
VCC (pin 1)    ───►   3.3V
DATA (pin 2)   ───►   GPIO4
GND (pin 3)    ───►   GND

Note: Most DHT11 modules have built-in 10kΩ pull-up.
      If using bare sensor, add 10kΩ between DATA and VCC.
```

#### Capacitive Soil Moisture Sensor
```
Soil Sensor           ESP32
───────────           ─────
VCC            ───►   3.3V (NOT 5V - damages sensor)
GND            ───►   GND
AOUT           ───►   GPIO34

Note: GPIO34 is ADC1_CH6. Using ADC2 pins causes WiFi conflicts.
      Sensor outputs 0-3V proportional to moisture.
```

#### Float Switch (Tank Level)
```
Float Switch          ESP32
────────────          ─────
Wire 1         ───►   GPIO5 (has internal pull-up)
Wire 2         ───►   3.3V

Configuration: INPUT_PULLUP mode
Behavior:
  - Water present → switch closed → GPIO5 reads HIGH
  - No water → switch open → GPIO5 reads LOW (pulled down)
```

#### Relay Module
```
Relay Module          ESP32           Pump Circuit
────────────          ─────           ────────────
VCC            ───►   VIN (5V)        
GND            ───►   GND             
IN             ───►   GPIO18          

NO (Normally Open)  ──────────────►   Pump positive
COM (Common)        ◄──────────────   5V power supply positive
                                      Pump negative → Power supply GND

Note: Relay coil needs 5V (from VIN), but trigger is 3.3V compatible.
      Using NO terminal = pump OFF when ESP32 boots/resets (safe default).
```

---

## Assembly Steps

### Step 1: Breadboard Layout (10 min)

**Goal**: Position components for clean wire routing

```
┌────────────────────────────────────────────────────────────────────┐
│                         BREADBOARD LAYOUT                          │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│    Power Rails (top)                                               │
│    ═══════════════════════════════════════════════                │
│    + (red)    3.3V rail                                           │
│    - (blue)   GND rail                                            │
│                                                                    │
│    Row 1-15:  ESP32 DevKit (centered, straddling middle gap)      │
│                                                                    │
│    Row 20-25: DHT11 module (left side)                            │
│                                                                    │
│    Row 20-25: Soil sensor wires (right side)                      │
│                                                                    │
│    Row 30-35: Float switch wires (left side)                      │
│                                                                    │
│    External:  Relay module (off-breadboard, connected via wires)  │
│                                                                    │
│    Power Rails (bottom)                                            │
│    ═══════════════════════════════════════════════                │
│    + (red)    5V rail (from VIN)                                  │
│    - (blue)   GND rail (shared)                                   │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
```

**Pro Tips**:
- Leave 3 rows empty between components for debugging access
- Use consistent wire colors: RED=power, BLACK=ground, others=signal
- Keep relay module off-breadboard (vibration and heat isolation)

### Step 2: Power Distribution (5 min)

1. Connect ESP32 `3.3V` → breadboard top `+` rail
2. Connect ESP32 `VIN` → breadboard bottom `+` rail (5V)
3. Connect ESP32 `GND` → both `-` rails (top and bottom)
4. Verify: Multimeter shows 3.3V on top rail, 5V on bottom rail

**Warning**: Never connect 5V to GPIO pins (max 3.3V). Only VIN and relay VCC use 5V.

### Step 3: DHT11 Connection (5 min)

1. Insert DHT11 module into breadboard (rows 20-22)
2. Connect VCC pin → 3.3V rail (NOT 5V)
3. Connect GND pin → GND rail
4. Connect DATA pin → ESP32 GPIO4

**Test Point**: After code upload, serial monitor should show temperature/humidity readings.

### Step 4: Soil Sensor Connection (5 min)

1. Route 3 wires from soil sensor to breadboard
2. Connect VCC → 3.3V rail
3. Connect GND → GND rail
4. Connect AOUT → ESP32 GPIO34

**Test Point**: Touch sensor probes with wet fingers → moisture reading should increase.

### Step 5: Float Switch Connection (5 min)

1. Float switch has 2 wires (normally open contact)
2. Connect one wire → ESP32 GPIO5
3. Connect other wire → 3.3V rail

**Test Point**: Tilt float switch up (simulating water present) → GPIO5 reads HIGH.

### Step 6: Relay Module Connection (10 min)

1. Mount relay module separately (not on breadboard)
2. Connect relay VCC → 5V rail (bottom)
3. Connect relay GND → GND rail
4. Connect relay IN → ESP32 GPIO18
5. **Pump circuit (isolated)**:
   - Pump positive wire → relay NO terminal
   - External 5V positive → relay COM terminal
   - Pump negative wire → External 5V negative

**Test Point**: Trigger GPIO18 HIGH → relay clicks, LED turns on.

### Step 7: Final Verification (5 min)

Before powering on, verify:

| Check | Expected |
|-------|----------|
| No shorts between 3.3V and GND rails | Multimeter shows OL (open) |
| No shorts between 5V and GND rails | Multimeter shows OL (open) |
| ESP32 orientation correct | USB port accessible |
| Relay IN connected to correct GPIO | GPIO18, not GPIO8 |
| Soil sensor on 3.3V, not 5V | Protects sensor from damage |

---

## First Power-On Checklist

1. **Connect USB** to ESP32 (powers board, provides serial)
2. **Open Arduino IDE** Serial Monitor (115200 baud)
3. **Upload code** and observe boot message
4. **Verify sensor readings**:
   - Soil moisture: 0-100% (not stuck at 0 or 100)
   - Temperature: Room temp ±5°C
   - Humidity: Reasonable for environment
   - Tank: Changes when float switch tilted
5. **Test relay manually**:
   - In serial monitor, wait for "WATER NOW" state
   - Or modify code temporarily to force pump ON
   - Confirm relay clicks and pump runs

---

## Mechanical Integration

### Soil Sensor Placement

```
                    ┌─────────────┐
                    │   PLANT     │
                    │    POT      │
                    │             │
          Soil line │ ~~~~~~~~~~~ │
                    │   ┌─────┐   │    ← Insert sensor probes 3-5cm deep
                    │   │probe│   │    ← Position near root zone (not edge)
                    │   │     │   │    ← Keep PCB ABOVE soil (moisture damage)
                    │   └──┬──┘   │
                    │      │      │
                    └──────┼──────┘
                           │
                    Wire to ESP32
```

**Important**: 
- Only metal probes touch soil, not the PCB
- Vertical insertion gives consistent readings
- Avoid placing directly against pot wall

### Pump & Tubing Setup

```
┌────────────┐        ┌──────────────┐        ┌────────────┐
│  WATER     │        │   SILICONE   │        │   PLANT    │
│  RESERVOIR │───────►│   TUBING     │───────►│   POT      │
│            │        │   (6mm ID)   │        │            │
│  ┌──────┐  │        └──────────────┘        │            │
│  │ PUMP │  │                                │            │
│  │      │  │        Avoid: Kinks, loops     │            │
│  └──────┘  │              that trap air     │            │
└────────────┘                                └────────────┘

Pump submerged          Tubing runs           Outlet above
in water                straight or           soil surface
                        gentle curve
```

**Tubing Tips**:
- Cut tubing to minimum length (reduces resistance)
- Secure outlet end (prevents movement during pump operation)
- Keep water reservoir higher than pump if possible (reduces priming issues)

### Float Switch Mounting

```
        WATER RESERVOIR
        ┌─────────────┐
        │             │ ← Mount switch ~2cm above pump intake
        │ ┌─────────┐ │    (ensures pump doesn't run dry)
        │ │  FLOAT  │ │
        │ │  SWITCH │ │
        │ └────┬────┘ │
        │      │      │ ← Float drops when water level falls
        │      │      │
        │  ┌───┴───┐  │
        │  │ PUMP  │  │
        │  └───────┘  │
        └─────────────┘
```

---

## Troubleshooting Hardware Issues

| Symptom | Likely Cause | Solution |
|---------|--------------|----------|
| ESP32 won't boot | Strapping pin conflict | Disconnect GPIO0, 2, 12, 15 during upload |
| DHT reads NaN | Cold sensor | Wait 2+ seconds after power-on |
| Soil always 0% | Sensor on 5V | Reconnect to 3.3V |
| Soil always 100% | Probe not in soil | Insert probes into medium |
| Relay won't click | Wrong relay VCC | Use 5V (VIN), not 3.3V |
| Pump won't run | NO/NC confusion | Use NO terminal for normally-off |
| Float always HIGH | Wiring reversed | Swap the two float switch wires |
| Erratic readings | Ground loop | Use star grounding (all GNDs to one point) |

---

## Upgrading to Permanent Installation

### From Breadboard to PCB

1. **Prototype verification**: Run on breadboard for 1 week minimum
2. **PCB layout**: Transfer connections to perfboard/protoboard
3. **Solder connections**: Use flux, tin wires before soldering
4. **Add headers**: For DHT11 and soil sensor (replaceable)
5. **Strain relief**: Hot glue on wire entry points
6. **Conformal coating**: Optional, for humidity protection

### Outdoor Enclosure Requirements

| Feature | Requirement | Notes |
|---------|-------------|-------|
| IP rating | IP65 minimum | Dust-tight, water jet protection |
| Cable glands | PG7 or M12 | For sensor and pump wires |
| Ventilation | Small vent with filter | Prevents condensation |
| Mounting | Wall bracket or stake | Secure against wind |
| Power | Weatherproof outlet or battery | Outdoor-rated extension |

---

*Assembly Guide Version: 1.0*  
*Last Updated: April 2026*
