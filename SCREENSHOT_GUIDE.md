# Wokwi Simulation Screenshot Guide

## Overview
This guide provides step-by-step instructions for capturing evidence screenshots from Wokwi simulator for IEEE paper figures and validation documentation.

**Required**: Wokwi simulator running with `simulation/wokwi/sketch.ino` compiled and executing

---

## Figure 3: Wokwi Circuit Snapshot

### Objective
Capture a clear overview of the simulated circuit showing all components and connections.

### Steps

1. **Start Simulator**
   ```
   > Task: Wokwi: Start Simulator
   ```
   Wait 5-10 seconds for circuit to display and serial output to begin.

2. **Maximize Wokwi Window**
   - Open Wokwi circuit viewer (should auto-launch in web view)
   - Expand window to full screen if available
   - Position for clarity: circuit diagram should be centered

3. **Zoom to Fit**
   - Use Wokwi's zoom controls to fit entire circuit in viewport
   - Keyboard: `Ctrl+0` (reset zoom) or `Ctrl+-` (zoom out)
   - Goal: All components visible without scrolling

4. **Component Visibility Check**
   - ✅ ESP32 DevKit clearly visible (left side)
   - ✅ DHT22 sensor (top right)
   - ✅ Soil sensor potentiometer (bottom)
   - ✅ Tank level switch (left side)
   - ✅ Relay module (right side)
   - ✅ Connection wires visible
   - ✅ LED indicator visible

5. **Capture Screenshot**
   - **Windows**: `Win+Shift+S` (snip) or Print Screen
   - **Web Browser**: Right-click → "Save Image As" on Wokwi circuit tab
   - Save as: **`docs/images/wokwi-circuit-snapshot.png`**
   - **Or**: Replace in existing screenshot if same layout

6. **Crop if Necessary**
   - Remove browser chrome/tabs from screenshot
   - Keep only circuit diagram and components visible
   - Recommended dimensions: 1200×800 px minimum

---

## Figure 4: Pump Active Validation Evidence

### Objective
Capture serial output and circuit state showing pump activation when soil moisture is below threshold.

### Prerequisites
- Wokwi simulator running with firmware executing
- Serial monitor visible (usually auto-opens at bottom)

### Steps

1. **Reset Simulator**
   - Stop and restart to get fresh baseline
   - Initial state: wet soil (high moisture %), pump OFF

2. **Verify Baseline** (should see serial output like):
   ```
   [000:00:10] Temp: 25.0°C, Humidity: 50%, Soil: 1850 ADC (65%), Tank: HIGH, Pump: OFF, Decision: hold_soil_ok
   [000:00:13] Temp: 25.0°C, Humidity: 50%, Soil: 1850 ADC (65%), Tank: HIGH, Pump: OFF, Decision: hold_soil_ok
   ```

3. **Trigger Dry Soil** (simulating drought)
   - In Wokwi, interact with the soil sensor potentiometer
   - Rotate dial toward "DRY" side (0 ADC equivalent)
   - Watch serial output: ADC value should drop, moisture % should drop

4. **Observe Pump Activation**
   - Wait 6+ seconds after dry soil trigger
   - Watch serial output for:
     ```
     [000:00:20] Temp: 25.1°C, Humidity: 49%, Soil: 2200 ADC (30%), Tank: HIGH, Pump: ON, Decision: watering_start
     [000:00:23] Temp: 25.1°C, Humidity: 49%, Soil: 2200 ADC (30%), Tank: HIGH, Pump: ON, Decision: pump_running_cooldown_pending
     ```
   - Observe in circuit: Relay should be engaged, LED may light

5. **Position for Screenshot**
   - Arrange windows so both visible:
     - Left: Wokwi circuit (showing relay ON state and LED)
     - Right: Serial monitor (showing pump ON in telemetry)
   - Or: Single tall screenshot with circuit top, serial output bottom

6. **Capture Screenshot**
   - Screenshot showing:
     - ✅ Soil ADC/% (below 60% threshold)
     - ✅ Tank: HIGH
     - ✅ Pump: ON
     - ✅ Decision: watering_start or pump_running
   - Save as: **`docs/images/pump-active-evidence.png`**
   - **Or**: Verify matches existing `pump-active.png`

---

## Figure 5: Tank Safety Interlock Evidence

### Objective
Capture evidence that pump is blocked when tank is empty, even with dry soil.

### Prerequisites
- Wokwi simulator running
- Previous pump ON state (optional; can start fresh)

### Steps

1. **Setup: Dry Soil Already Active**
   - Use same state as pump-active figure (dry soil, pump on)
   - Or: Manually set soil to low moisture % (≤30%)

2. **Trigger Tank Empty**
   - In Wokwi circuit, interact with tank level switch
   - Toggle switch to LOW/EMPTY position
   - Watch serial output: `tank_available: 0`

3. **Observe Pump Blocks**
   - Pump should stop within 3-6 seconds
   - Even though soil is dry, pump remains OFF
   - Serial output should show:
     ```
     [000:01:05] Temp: 25.2°C, Humidity: 48%, Soil: 2100 ADC (32%), Tank: LOW, Pump: OFF, Decision: hold_tank_empty
     [000:01:08] Temp: 25.2°C, Humidity: 48%, Soil: 2100 ADC (32%), Tank: LOW, Pump: OFF, Decision: hold_tank_empty
     ```

4. **Verify Tank Override Priority**
   - This proves safety interlock works: **TANK EMPTY > SOIL DRY**
   - Pump correctly blocked despite watering need

5. **Position for Screenshot**
   - Same layout as pump-active: circuit + serial
   - Highlight:
     - ✅ Soil: LOW (dry, normally triggers pump)
     - ✅ Tank: LOW
     - ✅ Pump: OFF (correctly blocked)
     - ✅ Decision: hold_tank_empty

6. **Capture Screenshot**
   - Save as: **`docs/images/tank-interlock-evidence.png`**
   - **Or**: Verify matches existing `safety-interlock.png`

---

## Figure 6: Cooldown Behavior Evidence

### Objective
Capture serial output showing cooldown delay between consecutive watering cycles.

### Prerequisites
- Wokwi simulator running with fresh baseline
- Serial monitor clearly visible with timestamps

### Steps

1. **Initial Setup**
   - Baseline: wet soil, pump OFF
   - Tank: HIGH (available)

2. **First Watering Cycle**
   - Trigger dry soil (set potentiometer low)
   - Wait for pump to activate
   - Watch serial output for `Decision: watering_start`
   - Note the timestamp when pump activates: **T1**

3. **Pump Auto-Stops**
   - Pump runs for ~5 seconds (PUMP_ON_DURATION_MS)
   - Pump stops automatically (duration timeout or watchdog)
   - Note timestamp when pump stops: **T2**

4. **Trigger Immediate Reactivation**
   - Immediately (while cooldown is active) set soil to dry again
   - Pump should NOT activate
   - Serial output should show:
     ```
     [000:00:30] ... Pump: OFF, Decision: cooldown_active (XX seconds remaining)
     ```
   - Note remaining cooldown: should show ~50-58 seconds (60 total minus elapsed)

5. **Wait for Cooldown Expiry**
   - Monitor serial output for cooldown countdown
   - After ~60 seconds total cooldown, soil still dry
   - Pump should automatically reactivate (new cycle)

6. **Capture Multi-Line Evidence**
   - Position serial output to show:
     - Time T1: watering_start (pump ON)
     - Time T1+5: pump stops, cooldown begins
     - Time T1+6-59: cooldown_active with countdown
     - Time T1+60+: watering_start again (reactivation)
   - This shows full cooldown cycle in one screenshot

   **Alternative**: If single screenshot insufficient, capture two:
   - Screenshot 1: watering_start + automatic pump stop
   - Screenshot 2: cooldown_active countdown
   - Screenshot 3: automatic reactivation after cooldown

7. **Capture Screenshots**
   - Save as: **`docs/images/cooldown-behavior-evidence.png`**
   - **Or**: Verify matches existing `cooldown-proof.png`
   - Include timestamp column to prove duration

---

## Figure 1: System Block Diagram (Specification)

**Cannot be captured directly from Wokwi.** This is a conceptual diagram to create separately.

### Specification
```
┌─────────────────────────────────────────────────────────────────┐
│                    Autonomous Irrigation System                  │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────┐         ┌─────────────────────┐
│  Sensing Layer   │         │  Decision Engine    │
├──────────────────┤         ├─────────────────────┤
│ • Soil Moisture  │──────→  │  Decision Logic     │──┐
│   (GPIO 34 ADC)  │         │  (3-layer safety)   │  │
│                  │         └─────────────────────┘  │
│ • Temperature    │────→                              │
│   (GPIO 4 DHT)   │         ┌─────────────────────┐  │
│                  │         │  Control Output     │←─┘
│ • Tank Level     │──→      ├─────────────────────┤
│   (GPIO 5 Dig)   │         │ Relay ON/OFF        │
└──────────────────┘         │ (GPIO 18)           │
                             └─────────────────────┘
                                      │
                                      ↓
                             ┌─────────────────┐
                             │  5V Pump Relay  │
                             │  (Pump Control) │
                             └─────────────────┘
                                      │
                                      ↓
                             ┌─────────────────┐
                             │  Water Pump     │
                             │  (Actuation)    │
                             └─────────────────┘

Safety Layers:
  1. Tank Interlock: Blocks pump if tank_available == 0
  2. Cooldown Delay: 60-second pause between cycles
  3. Watchdog Timer: Hard pump cutoff at 10 seconds
```

**To Create**: Use Graphviz, draw.io, Lucidchart, or similar tool and save as PNG.

---

## Figure 2: Irrigation Control Flowchart (Specification)

**Cannot be captured from Wokwi.** This is a logic flow diagram.

### Specification
```
START (Every 2 seconds)
    ↓
Read Sensors (Soil, DHT, Tank)
    ↓
[Tank = Empty?] ──YES──→ Pump = OFF; goto Cooldown Check
    ↓ NO
[Soil >= Threshold (60%)?] ──YES──→ Pump = OFF; goto Cooldown Check
    ↓ NO
    ↓
[Decision Counter < 3?] ──YES──→ Increment Counter; DO NOT ACT
    ↓ NO (3 consecutive dry readings = confirmed dry)
    ↓
[Cooldown Active?] ──YES──→ Pump = OFF; Decrement Cooldown Timer
    ↓ NO
    ↓
Pump = ON
    ↓
[Pump Runtime > 10 seconds (Watchdog)?] ──YES──→ Pump = FORCE OFF (safety)
    ↓ NO
    ↓
Output Telemetry (Sensors, Pump State, Decision)
    ↓
Wait 2 seconds
    ↓
(LOOP)
```

**To Create**: Use flowchart tool (Lucidchart, draw.io, Miro) and save as PNG.

---

## General Screenshot Tips

### Quality Standards
- **Resolution**: Minimum 1024×768 px, preferably 1200×900+
- **Legibility**: Text should be readable without zoom
- **Contrast**: Dark background with light text (or vice versa) for clarity
- **Format**: Save as PNG (lossless) for academic papers

### Screenshot Tools
| Tool | Platform | Command |
|------|----------|---------|
| Windows Screenshot | Windows | `Win+Shift+S` |
| macOS Screenshot | macOS | `Cmd+Shift+4` |
| Print Screen | Any | `Print Screen` key |
| Browser DevTools | Any | `F12` → Device View |
| Wokwi Export | Wokwi | Built-in export (if available) |

### Best Practices
1. **Close unnecessary windows** to reduce clutter
2. **Use consistent font size** across screenshots (zoom if needed)
3. **Include timestamps** in serial output to prove timing
4. **Annotate if unclear** (use arrows, circles) in post-processing
5. **Verify all text readable** before saving final version

---

## Documentation References

- See `docs/simulation-validation-checklist.md` for full validation procedure
- See `firmware/include/config.h` for exact threshold and timing values
- See `README.md` "Validation Evidence" section for where screenshots are used

---

## Next: Updating IEEE Paper

Once all screenshots captured and saved:

1. Update IEEE_REPORT_DRAFT.md Section 8 with actual figure filenames
2. Add captions under each figure with description
3. Cross-reference figures in text (e.g., "As shown in Figure 4...")
