# Prototype Development Log

This document captures the problems we hit while building this system, how we solved them, and what that taught us. Every safety feature in the firmware exists because we discovered it the hard way.

---

## DHT22 Warm-Up Issue

**What happened:** On the first boot, the DHT22 would send garbage or NaN values. We'd read the sensor in the initialization routine and get invalid data back.

**Why:** DHT22 needs time to stabilize after power-on. It's not just a dumb sensor—there's internal logic that needs time to wake up.

**Fix:** Added a 2-second wait before the first sensor read. Now the sensor has time to stabilize, and we get valid readings immediately.

**How we verify it works:** Serial telemetry shows clean temperature/humidity values by the 30-second mark. No garbage, no NaN.

---

## Soil Sensor Calibration

**What we learned:** Soil sensors are not absolute—they depend heavily on the specific soil, moisture content, and sensor placement. The capacitive sensor outputs ADC values from 0–4095, but what does "dry" actually mean in ADC numbers? Depends on your soil.

**Our reference values:**
- Our dry soil (just in air): ~3950 ADC
- Our saturated soil (fully wet): ~1650 ADC

**What you need to do:** Test your soil with your sensor before deployment. Stick it in dry soil and note the reading. Stick it in wet soil and note the reading. Use those values in `firmware/include/config.h`. If you use our values and it's wrong, blame your soil, not the system.

**Verification:** We tested across dry/wet states and saw smooth transitions in the ADC readings, no jumps or noise.

---

## Tank Switch Debounce

**What happened:** The float switch would bounce—contact made, contact broke, contact made again, all in milliseconds. The pump would turn on and off rapidly or get confused about what "empty" actually meant.

**Why:** Mechanical switches bounce when they first connect or disconnect. It's just physics.

**Fix:** Added debounce logic in firmware. The system waits for the signal to be stable for a few cycles before accepting a state change.

**How we verify:** Watch the serial telemetry. Tank empty should show a stable LOW signal, not flickering between HIGH and LOW.

---

## Relay and Pump Power Rails

**The big failure:** Early prototypes tried to power everything from the ESP32's supply. The pump inrush current was so high it caused the ESP32 to brownout mid-decision. The relay would open, interrupting the watering, and everything got confused. We'd lose sensor readings, lose control, lose the plot entirely.

**The solution:** Completely separate 5V supply for the pump and relay. Same ground between supplies (mandatory—if they're not grounded together, GPIO signals can't communicate), but independent power rails.

**Why it matters:** Pump inrush can be 2–3 amps instantaneously. The ESP32's regulator can't handle that. Separate supplies mean the pump can draw what it needs without dragging the microcontroller down with it.

**Verification:** System stays responsive. Pump turns on, telemetry keeps flowing, no brownouts or resets.

---

## Relay Oscillation Issue

**What happened:** In early tests, we sometimes saw the relay chattering—clicking on and off rapidly even when the pump should be running smoothly.

**Root cause:** The relay control line was sensitive to noise coupling from the high-current pump line running nearby. Even with good wiring, some noise would make GPIO18 bounce slightly, and that would trigger the relay.

**What we tried:**
- Added capacitor for noise filtering on the relay input
- Added debounce in firmware  
- Separated the control wiring from the power wiring as much as possible
- Added a cooldown period between pump cycles (this also prevents the pump from rapid-cycling anyway)

**Result:** No more oscillation. The relay is solid now, and the system cycles predictably.

---

## Complete System Validation

After all the individual fixes, we tested the entire flow:

**Dry soil test** — Soil sensor reads below threshold, DHT22 shows safe temperature/humidity, tank float says water available, pump turns on for 10 seconds, then stops and enforces cooldown. ✅ Works.

**Wet soil test** — Soil sensor reads above threshold, pump stays off. Keep it off. No watering even if temperature is perfect and tank is full. ✅ Works.

**Tank empty test** — Float switch says no water, pump hard-blocks. Will not run under any condition. Perfect. This prevents pump damage and is non-negotiable. ✅ Works.

**Cooldown test** — One watering cycle completes, pump stops, cooldown timer starts. Try to trigger immediately—pump stays off. Wait for cooldown (60 seconds in production, shorter in simulation), then it can run again. ✅ Works.

---

## What This All Means

The system is stable because we:
1. Fixed sensor initialization (warm-up)
2. Accepted that calibration is site-specific (measure your own soil)
3. Eliminated electrical bounce (debounce)
4. Separated power rails so the pump doesn't break the controller
5. Added noise immunity (filtering, debounce, cooldown)
6. Hard-blocked dangerous states (tank empty = never pump)

None of this was the first design. Every piece came from a failure, a test that failed, a discovery of what could go wrong. That's why we document it—it's real engineering, not theoretical.
