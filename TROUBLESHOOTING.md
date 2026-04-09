# Troubleshooting Guide

## Build & Compilation Issues

### Arduino CLI Not Found
**Error**: `"arduino-cli is not installed or not in PATH"`

**Solution**:
1. Install Arduino CLI: https://arduino.cc/pro/software
2. Add to PATH:
   - **Windows**: `setx PATH "%PATH%;C:\Users\{username}\AppData\Local\Arduino15\bin"`
   - Restart PowerShell after PATH update
3. Verify: `arduino-cli version`

---

### Board Not Recognized During Build
**Error**: `"Board 'esp32:esp32:esp32' not found"`

**Solution**:
1. Update board index:
   ```
   arduino-cli core update-index
   ```
2. Install ESP32 core:
   ```
   arduino-cli core install esp32:esp32@2.0.13
   ```
3. Verify: `arduino-cli board list`
4. If still missing, install manually:
   - Arduino IDE → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

---

### Library Not Found
**Error**: `"DHT sensor library for ESPx not found"`

**Solution**:
1. Search for library:
   ```
   arduino-cli lib search "DHT sensor"
   ```
2. Install:
   ```
   arduino-cli lib install "DHT sensor library for ESPx"
   ```
3. If specific version needed:
   ```
   arduino-cli lib install "DHT sensor library for ESPx@1.4.4"
   ```

---

### Build Takes Too Long / Hangs
**Symptom**: PowerShell freezes during build

**Solution**:
1. Increase timeout in `build-firmware.ps1`:
   - Change `$buildTimeout = 60` to `$buildTimeout = 120` (seconds)
2. Check disk space: Build can fail silently if low on space
3. Clear build cache:
   ```
   ./scripts/clean-build.ps1
   ```

---

## Wokwi Simulation Issues

### Simulator Won't Start
**Error**: `"Failed to launch Wokwi simulator"` or blank screen

**Solution**:
1. Verify VS Code has Wokwi extension installed:
   - `Ctrl+Shift+X` → Search "Wokwi" → Install
2. Clear Wokwi cache:
   - Close VS Code
   - Delete: `C:\Users\{username}\AppData\Local\Code\Cache\wokwi*`
   - Restart VS Code
3. Rebuild firmware:
   ```
   ./scripts/build-firmware.ps1
   ```
4. Check port conflicts: Wokwi typically uses port 3000-3010

---

### Firmware Compiles But Simulator Shows Wrong Behavior
**Symptom**: Pump never activates, sensors show garbage values

**Solution**:
1. Verify ELF file updated after build:
   - Check timestamp: `simulation/wokwi/build/sketch.ino.elf`
   - Should be recent (within last minute)
2. Check diagram.json connections:
   - GPIO 34 connected to potentiometer? 
   - GPIO 4 connected to DHT?
   - GPIO 18 connected to relay?
3. Restart simulator:
   - Click ⟳ in Wokwi web interface
   - Or: `Ctrl+C` in terminal, run build + start again
4. Verify telemetry output:
   - Serial output starting with timestamps?
   - If not, check baud rate (should be 115200)

---

### Serial Monitor Shows No Output
**Error**: Terminal blank or only garbage characters

**Solution**:
1. Verify baud rate: **Must be 115200**
   - Arduino IDE: Bottom-right dropdown
   - VS Code: Select correct port + baud rate in Serial Monitor
2. Check firmware `telemetry.h` is being included:
   - Look for `#include "telemetry.h"` in `main.ino`
3. Ensure Serial.begin() called:
   - Should be first line in setup(): `Serial.begin(115200);`
4. Reset board:
   - Unplug USB, wait 3 seconds, replug
   - Or: Press EN button on ESP32 board

---

### Serial Output Shows Only `0xFF` or Garbage
**Cause**: Baud rate mismatch

**Solution**:
1. **CRITICAL**: Check exact baud rate in code:
   ```cpp
   Serial.begin(115200);  // This line in main.ino
   ```
2. Change serial monitor baud to match (usually bottom-right dropdown)
3. Reset board after changing baud rate

---

## Sensor Reading Issues

### Soil Sensor Always Reads 0 or 4095
**Cause**: ADC pin not connected or disconnected mid-run

**Solution**:
1. **Wokwi**: Verify potentiometer connected to GPIO 34:
   - Check `diagram.json`: potentiometer "to" should point to ESP32 GPIO 34
2. **Hardware**: 
   - Check GPIO 34 jumper connection (often loose)
   - Try wiggling jumper
   - Try different breadboard row
3. Test ADC directly:
   ```cpp
   Serial.println(analogRead(34));  // Should print 0-4095 when moved
   ```

---

### DHT22 Sensor Constantly Shows "Read Failed"
**Cause**: Power supply too low, loose connection, or unsupported sensor

**Solution**:
1. Verify power: DHT22 needs stable 3.3V (not 5V)
   - Use voltmeter to check actual voltage at DHT pins
   - If <3.2V, upgrade power supply
2. Add pull-up resistor if not built into library:
   - 10kΩ pull-up on DATA line (GPIO 4 to 3.3V)
3. Check sensor type:
   - Code uses: `DHT dht(DHT_PIN, DHT22);`
   - Verify actual sensor is DHT22 (not DHT11)
4. Try different GPIO:
   - Temporarily change `#define DHT_PIN 2` and test
   - GPIO 2 often more stable than GPIO 4

---

### Tank Level Switch Reading Stuck at 0 or 1
**Cause**: Float switch not moving, no connection, or noisy signal

**Solution**:
1. **Wokwi**: Move slide switch manually in simulator view
2. **Hardware**: 
   - Check float physically moves up/down (not stuck)
   - Ensure switch pins connected to GPIO 5 and GND
3. Add debouncing if not working:
   - Code already has 50ms debounce in `config.h`
   - If still noisy, increase: `#define TANK_DEBOUNCE_MS 100`
4. Test with digital read:
   ```cpp
   Serial.println(digitalRead(5));  // Should toggle between 0 and 1
   ```

---

## Pump & Relay Issues

### Relay Never Engages (No Clicking Sound)
**Cause**: GPIO 18 not toggling, relay needs different voltage, or physical damage

**Solution**:
1. Check relay power:
   - Relay VCC must be 5V (not 3.3V)
   - Use voltmeter to verify 5V at relay VCC pin
2. Test GPIO 18 directly:
   ```cpp
   pinMode(18, OUTPUT);
   digitalWrite(18, HIGH);   // Relay should click
   delay(1000);
   digitalWrite(18, LOW);    // Relay should release
   ```
3. Verify relay polarity:
   - Some relays are active-HIGH (click when HIGH)
   - Some are active-LOW (click when LOW)
   - Code assumes active-HIGH: `#define RELAY_ON HIGH`
   - **Wokwi relay IS active-low**: Code handles this with conditional
4. Physical relay check:
   - Try directly powering relay VCC with batteries
   - If relay clicks manually, wiring issue; if not, relay may be broken
5. Replace relay if damaged

---

### Pump Runs Continuously (Won't Stop)
**Cause**: Watchdog not triggering or GPIO 18 stuck HIGH

**Solution**:
1. **Immediate**: Manually turn off water supply and power down ESP32
2. Check firmware watchdog:
   - `PUMP_WATCHDOG_MS = 10000` (10 seconds max)
   - After 10 seconds, pump should FORCE OFF
3. Verify GPIO 18 can toggle:
   ```cpp
   digitalWrite(18, LOW);  // Force pump off
   delay(2000);
   digitalWrite(18, HIGH); // Force pump on
   ```
4. Check for stuck relay:
   - Hear clicking when relay should toggle?
   - If silent, relay may be mechanically stuck
5. Update watchdog timeout as needed:
   - For testing: `#define PUMP_WATCHDOG_MS 5000` (5 seconds)
   - For production: keep at 10000 (10 seconds)

---

### Pump Runs But Very Weakly
**Cause**: Low current, low voltage, or pump cavitation

**Solution**:
1. Check power supply voltage:
   - Should maintain 5V even under pump load
   - If drops below 4.5V, pump may not prime
2. Ensure common ground between ESP32 and pump circuit
3. Check water lines for:
   - Blockages or kinks
   - Air locks (if underwater intake)
   - Clogged intake filter
4. Verify tubing sizes match pump spec (usually 8-10mm ID)

---

## State Machine / Logic Issues

### Pump Activates Even When Soil is Wet
**Cause**: Threshold set too low (sensor drift)

**Solution**:
1. Check threshold vs actual reading:
   - Read config: `#define SOIL_DRY_THRESHOLD_PERCENT 60`
   - Current reading should be clearly above 60% when wet
2. Recalibrate sensor:
   - Place in dry air: should read ~3950 ADC (= 0%)
   - Place in water: should read ~1650 ADC (= 100%)
   - If different, update in `config.h`:
     ```cpp
     #define SENSOR_ADC_DRY 3950    // Air dry value
     #define SENSOR_ADC_WET 1650    // Water saturated value
     ```
3. Increase threshold if sensor drifts:
   - Change to 65% or 70% to add safety margin
   - Recompile and test

---

### Pump Won't Activate Even When Soil is Dry
**Cause**: Decision debounce requires 3 consecutive readings

**Solution**:
1. Understand debouncing: 
   - Requires 3 consecutive dry readings (6 seconds minimum)
   - Single dry dip won't trigger pump
2. Wait longer after setting soil to dry:
   - Set dry, then wait ≥6 seconds before expecting activation
3. Check tank level:
   - Tank interlock has priority
   - If tank shows LOW/empty, pump won't activate (safety feature)
4. Verify cooldown isn't active:
   - If just watered, wait 60 seconds before next watering
   - Check serial output for "Decision: cooldown_active"

---

### Cooldown Timer Not Counting Down
**Cause**: Cooldown logic not triggering or stuck

**Solution**:
1. Verify pump recently completed cycle:
   - Should see `Decision: pump_running` in telemetry
   - Then `Decision: cooldown_active` immediately after
2. Check cooldown duration in config:
   - `#define PUMP_COOLDOWN_MS 60000` (should be 60 seconds)
3. Test pump manually:
   - Trigger dry soil, let pump run for 5 seconds
   - Then manually set soil to dry again
   - Should see "cooldown_active" with remaining time (should be ~55 sec)

---

## Hardware-Specific Issues

### ESP32 Keeps Resetting
**Symptom**: Serial output restarts repeatedly every few seconds

**Cause**: Brownout (power supply too weak) or software crash

**Solution**:
1. Check power supply:
   - Use USB 2.0 port (more current than USB 3.0 on some laptops)
   - Try different USB cable
   - Use powered USB hub if available
2. Reduce power draw:
   - Disable extra components temporarily
   - Test with just ESP32 and DHT first
3. Look for watchdog triggered reset:
   - If resets after 10 seconds consistently, watchdog may be active
   - Check `PUMP_WATCHDOG_MS` in config
4. Enable serial debug to see crash reason:
   - Add `esp_log_level_set("*", ESP_LOG_VERBOSE);` in setup()

---

### Temperature/Humidity Readings Impossible (999°C, etc.)
**Cause**: DHT sensor not responding or library bug

**Solution**:
1. Verify sensor is DHT22 (not DHT11 or AM2302)
   - Code: `DHT dht(DHT_PIN, DHT22);`
2. Check power: DHT22 critical on exact 3.3V
3. Try a different GPIO:
   - Change `#define DHT_PIN 4` to `#define DHT_PIN 2`
4. Update library:
   ```
   arduino-cli lib upgrade "DHT sensor library for ESPx"
   ```
5. Test with simpler code:
   ```cpp
   float temp = dht.readTemperature();
   if (isnan(temp)) {
     Serial.println("DHT read failed!");
   } else {
     Serial.println(temp);
   }
   ```

---

## Filesystem & Memory Issues

### "Sketch too large" or Out of Memory Error
**Cause**: Firmware size exceeds ESP32 flash

**Solution**:
1. Check sketch size:
   - Compiler output should show: "Sketch uses X bytes (Y% of 1441792 available)"
   - If >80%, need to optimize
2. Remove unused code/comments
3. Use PROGMEM for large strings:
   ```cpp
   const char msg[] PROGMEM = "Long string...";
   ```

---

## General Diagnostics

### "It Just Doesn't Work" - Where to Start
1. **Serial output first**:
   - If no serial output: USB/power issue
   - If garbage: baud rate mismatch
   - If stopping: watchdog or crash
2. **Then check each sensor independently**:
   - Soil: `Serial.println(analogRead(34));`
   - DHT: `Serial.println(dht.readTemperature());`
   - Tank: `Serial.println(digitalRead(5));`
3. **Then check relay & pump**:
   - `digitalWrite(18, HIGH);` should click relay and start pump
4. **Finally check logic**:
   - Trace through state machine with debug prints

---

## Need More Help?

- **Wokwi Issues**: https://docs.wokwi.com/
- **Arduino CLI**: https://arduino.github.io/arduino-cli/
- **ESP32 Documentation**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/
- **DHT Sensor Library**: https://github.com/beegee-tokyo/DHT
