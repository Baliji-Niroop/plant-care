/**
 * ESP32 Smart Irrigation Controller
 * 
 * SYSTEM OVERVIEW:
 * Autonomous soil-moisture-driven watering with multi-layer safety interlocks.
 * Designed for single-zone irrigation with 500ml reservoir and 12cm pot.
 * 
 * OPERATION CYCLE (2-second sensor polling):
 * 1. Read soil moisture, temperature, humidity, tank level
 * 2. Evaluate watering decision (must pass 5 independent checks)
 * 3. Activate pump for 5 seconds if all conditions met
 * 4. Enter 60-second cooldown to prevent pump thermal damage
 * 
 * SAFETY ARCHITECTURE:
 * - Watchdog timer: Force-stops pump after 10sec (2× normal runtime)
 * - Debounce filter: Requires 3 consecutive dry readings (6sec window)
 * - Tank interlock: Hardware cutoff when water unavailable
 * - Environmental bounds: Pauses operation in extreme temp/humidity
 * - Cooldown enforcer: Prevents rapid cycling (pump rated 20% duty cycle)
 * 
 * STATE MACHINE:
 * MONITORING → [conditions met] → WATERING → [5sec elapsed] → COOLDOWN → [60sec] → MONITORING
 *      ↓                              ↓
 *   [sensor fail]              [watchdog trigger]
 *      ↓                              ↓
 *   ERROR ←─────────────────────────────┘
 * 
 * CALIBRATION NOTES:
 * - Soil sensor: 4095 (air) → 0 (water). Adjust map() in sensors.h for your sensor.
 * - Pump runtime: ~30ml/sec flow rate. Recalculate for different pump models.
 * - Thresholds tuned for loamy soil + tropical houseplants.
 * 
 * POWER CONSUMPTION:
 * - Idle: ~80mA @ 5V (ESP32 + sensors)
 * - Pumping: ~350mA @ 5V (5-second bursts)
 * - Daily average: ~2.5Wh (10+ days on 5000mAh power bank)
 * 
 * AUTHOR: Niroop Baliji
 * PROJECT: IoT + Embedded Systems Portfolio
 * LICENSE: MIT
 */

#include "include/config.h"
#include "include/sensors.h"
#include "include/irrigation.h"
#include "include/telemetry.h"

Sensors sensors;
IrrigationController irrigation;
Telemetry telemetry;

SystemState systemState = STATE_MONITORING;
unsigned long lastSensorReadMs = 0;
SensorData currentData{};
IrrigationDecision lastDecision{};

void setup() {
  telemetry.begin();
  sensors.begin();
  irrigation.begin();
}

void loop() {
  // Watchdog check - catches relay stuck-on before thermal damage
  irrigation.update();

  // Sensor polling at 2-second intervals (configurable in config.h)
  const bool dueForRead = (millis() - lastSensorReadMs) >= SENSOR_READ_INTERVAL_MS;
  if (dueForRead) {
    currentData = sensors.read();
    lastSensorReadMs = millis();
    lastDecision = irrigation.evaluate(currentData);
  }

  /**
   * STATE MACHINE TRANSITIONS:
   * - MONITORING: Evaluate 5-check decision logic every cycle
   * - WATERING: Pump active, wait for duration timer or watchdog
   * - COOLDOWN: Mandatory rest period for pump thermal protection
   * - ERROR: Sensor failure detected, pump disabled, await recovery
   */
  switch (systemState) {
    case STATE_MONITORING:
      if (lastDecision.shouldWater) {
        const bool started = irrigation.startPump();
        if (started) {
          systemState = STATE_WATERING;
          Serial.println("[event] Pump started");
        }
      } else if (!sensors.healthy()) {
        systemState = STATE_ERROR;
      }
      break;

    case STATE_WATERING:
      if (!irrigation.isRunning()) {
        systemState = STATE_COOLDOWN;
        Serial.println("[event] Pump stopped");
      }
      break;

    case STATE_COOLDOWN:
      if (!irrigation.inCooldown()) {
        systemState = STATE_MONITORING;
      }
      if (irrigation.state() == PUMP_WATCHDOG_TRIGGERED) {
        systemState = STATE_ERROR;
      }
      break;

    case STATE_ERROR:
      irrigation.stopPump();
      Serial.println("[warn] Sensor instability detected, waiting for recovery");
      if (sensors.healthy()) {
        Serial.println("[info] Sensors recovered, returning to monitoring");
        systemState = STATE_MONITORING;
      }
      break;
  }

  telemetry.periodicReport(currentData, lastDecision, systemState, irrigation);
  
  // 100ms loop delay - balances responsiveness with power consumption
  // Trade-off: Slightly slower UI response vs. stable sensor readings
  delay(100);
}
