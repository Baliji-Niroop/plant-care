# Production Firmware

This directory contains the firmware that runs on physical ESP32 hardware. It implements the 7-stage decision logic, sensor acquisition, safety mechanisms, and telemetry that make the system work.

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
- The 7-stage decision hierarchy (checks all constraints)
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
```cpp
constexpr uint8_t PIN_SOIL_SENSOR = 34;   // Analog input for soil sensor
constexpr uint8_t PIN_DHT = 4;            // DHT22 data pin
constexpr uint8_t PIN_TANK_SWITCH = 5;    // Float switch pin
constexpr uint8_t PIN_RELAY = 18;         // Relay input pin
```
If your board has different pin assignments, edit these.

**Sensor calibration:**
Calibration references are managed dynamically at runtime via NVS (Non-Volatile Storage). On boot, the system waits 10 seconds for user calibration commands to save dry/wet references. The defaults (if no calibration is stored) are defined in `include/calibration.h`:
```cpp
static constexpr int DEFAULT_SOIL_RAW_DRY = 3950;
static constexpr int DEFAULT_SOIL_RAW_WET = 1650;
```
For more information, see `docs/04_calibration.md`.

**Decision thresholds:**
```cpp
constexpr int SOIL_DRY_THRESHOLD_PERCENT = 60; // Water below this moisture level

constexpr float TEMP_MIN_C = 15.0f;            // Don't water below this (°C)
constexpr float TEMP_MAX_C = 40.0f;            // Don't water above this (°C)
constexpr float HUMIDITY_MIN_PERCENT = 30.0f;  // Don't water below this (%)
constexpr float HUMIDITY_MAX_PERCENT = 80.0f;  // Don't water above this (%)
```

**Timing parameters:**
```cpp
constexpr uint8_t DECISION_DEBOUNCE_READINGS = 3;  // Dry readings needed to trigger pump
constexpr unsigned long SENSOR_READ_INTERVAL_MS = 2000UL; // Loop cycle frequency
constexpr unsigned long PUMP_ON_DURATION_MS = 5000UL;     // Pump runtime per cycle (5s)
constexpr unsigned long PUMP_COOLDOWN_MS = 60000UL;       // Cooldown before next run (60s)
constexpr unsigned long PUMP_WATCHDOG_MS = 10000UL;       // Max safety pump runtime limit (10s)
```

**Polarity & Electrical settings:**
```cpp
constexpr uint8_t RELAY_ON = HIGH;           // Output level to trigger relay (HIGH/LOW)
constexpr uint8_t RELAY_OFF = LOW;
constexpr uint8_t TANK_SWITCH_PIN_MODE = INPUT_PULLUP;
constexpr uint8_t TANK_WATER_PRESENT_LEVEL = LOW; // GPIO level when water is present
```
These depend on your specific relay module and float switch wiring.

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
4. Verify the system behaves as expected per `docs/06_validation.md`

---

## Understanding the Telemetry Output

The serial output is structured and easy to read. A typical periodic report printed to the Serial Monitor every 3 seconds looks like this:

```text
========== SYSTEM STATUS ==========
Uptime: 45s
--- Sensor Snapshot ---
Reading: valid
Soil: 42.5% [dry]
Temp: 24.2 C
Humidity: 55.8 %
Tank: available

--- Decision ---
Action: WATER NOW
Reason: Ready to water

Mode: watering
Pump: running (2s)
```

This tells you:
- System uptime is 45 seconds.
- Soil moisture is 42.5%, which is below the dry threshold (60%).
- Temperature (24.2°C) and Humidity (55.8%) are within safe bounds.
- Water tank is full/available.
- Decision: Trigger watering.
- Mode: Currently in `STATE_WATERING`.
- Pump: Running (actively pumping for 2 seconds).

If the pump doesn't turn on, you can look at the **Decision -> Reason** line to see exactly which safety check failed.

---

## Important Notes

**DO NOT flash this to the Wokwi simulation.** The simulation runs different firmware (`simulation/wokwi/sketch.ino`) with different timing and parameters. They're intentionally different and incompatible.

**Verify calibration before unattended deployment.** The default calibration references in the code are based on standard capacitive sensors. Your specific sensor may drift or act differently. Use procedures in `docs/04_calibration.md` to calibrate the sensor on-site before deploying it.

**Watchdog timeout is fixed at 10 seconds for safety.** If you change `PUMP_WATCHDOG_MS`, make sure it is high enough that the pump won't trigger it during normal 5-second operation (`PUMP_ON_DURATION_MS`), but low enough to catch runaway conditions quickly. We recommend leaving it at 10 seconds.

---

## If Something Goes Wrong

**Pump won't turn on:** Check the telemetry output. Look for which constraint is failing under the `--- Decision ---` section. If it's soil threshold, your sensor may need recalibration. If it's tank empty, check the float switch. If it's cooldown, just wait—that's intentional.

**Telemetry is garbage:** Verify serial baud rate is 115200. Try unplugging and replugging the USB cable. If still bad, verify the ESP32 is detected using `arduino-cli board list`.

**Sensor reads are always the same:** The sensor might be disconnected, or the ADC input isn't working. Check GPIO34 wiring.

**Watchdog keeps triggering:** The pump is running longer than 10 seconds. Check if the relay is stuck, or if you changed `PUMP_ON_DURATION_MS` to be longer than `PUMP_WATCHDOG_MS`.

For more troubleshooting, see `docs/07_troubleshooting.md`.

---

## Related Documentation

- **How the system makes decisions:** `docs/01_architecture.md`
- **Sensor calibration:** `docs/04_calibration.md`
- **Hardware pin mapping:** `hardware/README.md`
- **Testing and validation:** `docs/06_validation.md`
- **Deployment procedures:** `docs/05_deployment.md`

