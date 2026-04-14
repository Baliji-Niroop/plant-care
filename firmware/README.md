# Production Firmware

This directory contains the firmware that runs on physical ESP32 hardware. It implements the 6-stage decision logic, sensor acquisition, safety mechanisms, and telemetry that make the system work.

## What This Code Does

The firmware is responsible for four things:

**Sensor acquisition:** Every 5 seconds (roughly), it reads soil moisture from GPIO34, temperature/humidity from the DHT22 on GPIO4, and tank level from the float switch on GPIO5. It validates that each reading makes sense (is the ADC in range? did the sensor respond?).

**Decision making:** Once we have all sensor data, the system checks six constraints in order. Any single failure blocks the pump. The checks are: sensor frame valid, soil dry, debounce expired, temperature/humidity safe, tank has water, cooldown not active.

**Pump control:** If all six pass, GPIO18 goes HIGH and the relay closes. The pump runs. A watchdog timer monitors runtime—if it exceeds 30 seconds, emergency stop. After the pump stops, 60-second cooldown kicks in.

**Telemetry:** Every cycle, serial output (115200 baud) streams the readings, decision logic results, and any events. You can watch the system think in real-time.

## File Organization

**`firmware.ino`** — Main sketch and entry point
- Initializes hardware (GPIO, serial, sensors)
- Main control loop that orchestrates everything
- Implements the state machine for pump lifecycle

**`include/config.h`** — All configuration lives here
- GPIO pin numbers for your board
- Sensor calibration values (dry ADC, wet ADC)
- Decision thresholds (temperature min/max, humidity min/max)
- Timing parameters (cooldown duration, debounce, pump runtime, watchdog limit)
- Polarity settings (relay active-high or active-low, tank switch logic)

**`include/sensors.h`** — Sensor acquisition module
- DHT22 initialization and warm-up logic (waits 2 seconds after boot before reading)
- Analog ADC acquisition from GPIO34 (reads soil moisture)
- Tank switch debounce and state management (avoids false triggers)
- Frame validity checking (validates all readings before use)

**`include/irrigation.h`** — Decision logic and pump control
- The 6-stage decision hierarchy (checks all constraints)
- Relay control (GPIO18 output)
- Cooldown timer and enforcement
- Watchdog timer implementation
- Error state handling

**`include/telemetry.h`** — Serial output formatting
- Formats and sends diagnostic data over serial (115200 baud)
- Real-time sensor readings
- Decision logic results (why did we pump or not pump?)
- Event logging (pump activation, watchdog trigger, errors)

---

## The Decision Logic (Six Stages)

The system checks these in order. Any failure means pump stays OFF:

1. **Is the sensor frame valid?** 
   - Did we get responses from all sensors?
   - Are the ADC values in reasonable range?
   - If anything is garbage, pump OFF and keep waiting.

2. **Is soil actually dry?**
   - We've calibrated the sensor: dry (air) is ~3950 ADC, wet (saturated soil) is ~1650 ADC.
   - If soil is above the dry threshold, it's wet enough. Pump OFF.

3. **Have we debounced long enough?**
   - Soil readings bounce around the threshold when conditions are right on the edge.
   - We wait 5 seconds (configurable) after crossing the threshold before deciding.
   - Prevents the pump from clicking on and off.

4. **Is temperature and humidity safe?**
   - Example bounds: 10°C to 35°C for temperature, 30% to 80% for humidity (all configurable).
   - Watering outside these ranges can damage roots.
   - Out of range? Pump OFF.

5. **Does the tank have water?**
   - Float switch on GPIO5 is a hard interlock.
   - If tank is empty (switch HIGH), pump cannot run.
   - This prevents damage from dry-running the pump.

6. **Are we not in cooldown?**
   - After each 10-second watering cycle, we enforce a 60-second wait.
   - Prevents rapid cycling that stresses the pump and drowns the plant.
   - If cooldown is still active, pump OFF.

If all six pass, GPIO18 goes HIGH and the relay closes.

---

## Safety Mechanisms

Every safety layer came from hitting real problems:

- **Fail-safe hold:** Invalid sensor data stops the pump immediately and permanently (until data becomes valid again). Garbage data is worse than no watering.

- **Tank interlock:** Prevents dry-running damage. This is hard-wired in logic.

- **Debounce:** Eliminates the pump clicking on and off repeatedly when soil is right at the threshold.

- **Environmental guards:** Cold water or extreme humidity can damage roots. We don't water in those conditions.

- **Cooldown:** Early prototypes would trigger the pump 10 times in a row and drown plants. Cooldown prevents this.

- **Watchdog timer:** If the pump somehow runs longer than 30 seconds (safety limit), an emergency stop is triggered. Catches runaway conditions.

- **Error recovery:** If the system detects an error (sensor disconnected, bad ADC read), the pump goes OFF and stays OFF until conditions recover.

---

## Configuration

Everything tunable lives in `include/config.h`. You should only need to change this file in normal use:

**GPIO pin numbers:**
```c
#define PIN_SOIL_SENSOR 34      // ADC input, analog
#define PIN_DHT22_DATA 4        // Digital 1-wire protocol
#define PIN_TANK_FLOAT_SWITCH 5 // Digital input, pulled high
#define PIN_RELAY_CONTROL 18    // Digital output to relay
```
If your board has different pin assignments, edit these.

**Sensor calibration:**
```c
#define SOIL_DRY_REFERENCE 3950    // ADC reading when sensor in air
#define SOIL_WET_REFERENCE 1650    // ADC reading when sensor in saturated soil
```
These are the mapping points. If your specific sensor reads differently, update these values using procedures in `docs/calibration/calibration.md`.

**Decision thresholds:**
```c
#define TEMP_MIN_SAFE 10     // Don't water below this (°C)
#define TEMP_MAX_SAFE 35     // Don't water above this (°C)
#define HUMIDITY_MIN 30      // Don't water below this (%)
#define HUMIDITY_MAX 80      // Don't water above this (%)
```

**Timing parameters:**
```c
#define DHT_WARMUP_DELAY_MS 2000   // DHT22 needs 2 seconds post-boot
#define DEBOUNCE_DURATION_MS 5000  // Wait 5 seconds when soil near threshold
#define PUMP_RUNTIME_MS 10000      // Run pump for 10 seconds per cycle
#define COOLDOWN_DURATION_MS 60000 // Wait 60 seconds before next cycle
#define WATCHDOG_TIMEOUT_MS 30000  // Force stop if pump runs longer than 30 seconds
```

**Polarity settings:**
```c
#define RELAY_ACTIVE_HIGH true    // true = pump on when GPIO18 HIGH; false = pump on when GPIO18 LOW
#define TANK_SWITCH_LOW_MEANS_WATER true  // true = water present when GPIO5 LOW; false = water present when GPIO5 HIGH
```
These depend on your specific relay and float switch. Check your component datasheets.

After changing any of these, rebuild and reflash the firmware.

---

## Building and Flashing

**Prerequisites:**
- Arduino IDE or Arduino CLI with ESP32 core installed
- USB cable for ESP32 programming

**Build using Arduino CLI:**
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/firmware.ino
```

Should compile without warnings. If you get warnings about unused variables, that's fine (the preprocessor removes code for unused config values). If you get actual errors, something's wrong.

**Flash to hardware:**
```bash
arduino-cli upload --fqbn esp32:esp32:esp32 -p COM3 firmware/firmware.ino
```
Replace `COM3` with your actual serial port (use `arduino-cli board list` to find it).

**After flashing:**
1. Open serial monitor at 115200 baud (Arduino IDE: Tools → Serial Monitor)
2. You should see telemetry output immediately
3. Watch for sensor readings and decision logic results
4. Verify the system behaves as expected per `docs/validation/simulation-validation-checklist.md`

---

## Understanding the Telemetry Output

The serial output is human-readable and designed for real-time monitoring. A typical line looks like:

```
SENSOR: soil=2100 temp=22.5 hum=65.2 tank=OK | DECISION: PUMP_ON | EVENT: Pump started (10s runtime)
```

This tells you:
- Soil ADC is 2100 (dry)
- Temperature is 22.5°C (safe)
- Humidity is 65.2% (safe)
- Tank has water
- Decision was to turn on the pump
- Event: pump activated and will run for 10 seconds

If the pump doesn't turn on, look at the telemetry. You'll see which constraint failed.

---

## Important Notes

**DO NOT flash this to the Wokwi simulation.** The simulation runs different firmware (`simulation/wokwi/sketch.ino`) with different timing and parameters. They're intentionally different and incompatible.

**Verify calibration before unattended deployment.** The calibration references in `config.h` are based on reference sensors. Your specific sensor may drift over time or behave differently. Use procedures in `docs/calibration/calibration.md` to verify and adjust thresholds before trusting the system with plants that matter.

**Watchdog timeout is fixed at 30 seconds for safety.** If you change `WATCHDOG_TIMEOUT_MS`, keep it high enough that the pump won't trigger it during normal 10-second operation, but low enough to catch runaway conditions. We recommend leaving it at 30 seconds.

---

## If Something Goes Wrong

**Pump won't turn on:** Check the telemetry output. Look for which constraint is failing. If it's soil threshold, your sensor may need recalibration. If it's tank empty, check the float switch. If it's cooldown, just wait—that's intentional.

**Telemetry is garbage:** Verify serial baud rate is 115200. Try unplugging and replugging the USB cable. If still bad, verify the ESP32 is detected using `arduino-cli board list`.

**Sensor reads are always the same:** The sensor might be disconnected, or the ADC input isn't working. Check GPIO34 wiring.

**Watchdog keeps triggering:** The pump is running longer than 30 seconds. Either the pump is stuck, or the relay isn't closing properly. Check relay wiring and polarity settings.

For more troubleshooting, see `docs/troubleshooting/assembly-notes.md`.

---

## Related Documentation

- **How the system makes decisions:** `docs/architecture/architecture.md`
- **Sensor calibration:** `docs/calibration/calibration.md`
- **Hardware pin mapping:** `hardware/README.md`
- **Testing and validation:** `docs/validation/simulation-validation-checklist.md`
- **Deployment procedures:** `docs/deployment/deployment.md`

