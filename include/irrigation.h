#ifndef IRRIGATION_H
#define IRRIGATION_H

#include <Arduino.h>
#include "config.h"
#include "sensors.h"

/**
 * PUMP STATE MACHINE
 * Tracks pump lifecycle with explicit failure state.
 */
enum PumpState {
  PUMP_OFF = 0,                  // Idle, ready to start
  PUMP_RUNNING = 1,              // Active, timer counting
  PUMP_WATCHDOG_TRIGGERED = 2   // Emergency stop, requires inspection
};

/**
 * IRRIGATION DECISION STRUCTURE
 * Contains all 5 checks evaluated for watering decision.
 * Allows telemetry to report exactly why watering was blocked.
 */
struct IrrigationDecision {
  bool shouldWater;       // Final decision (all checks must pass)
  bool soilIsDry;         // Check 1: Below threshold
  bool envIsSafe;         // Check 2: Temp/humidity in range
  bool tankHasWater;      // Check 3: Float switch active
  bool cooldownExpired;   // Check 4: Rest period complete
  String reason;          // Human-readable decision explanation
};

/**
 * IRRIGATION CONTROLLER
 * 
 * DECISION ALGORITHM (5-check AND logic):
 * 1. Soil moisture below threshold (60% default)
 * 2. Temperature within safe range (15-40°C)
 * 3. Humidity above minimum (30%)
 * 4. Tank has water (float switch HIGH)
 * 5. Cooldown period expired (60 seconds)
 * 6. Debounce passed (3 consecutive dry readings)
 * 
 * SAFETY LAYERS:
 * - Runtime watchdog: Force-stop after 10 seconds
 * - Cooldown enforcer: Minimum 60 seconds between cycles
 * - Debounce filter: Prevents single-reading false triggers
 */
class IrrigationController {
 private:
  unsigned long lastWaterTime_;        // For cooldown calculation
  uint8_t consecutiveDryReadings_;     // Debounce counter
  PumpState pumpState_;
  unsigned long pumpStartTime_;

 public:
  IrrigationController()
      : lastWaterTime_(0),
        consecutiveDryReadings_(0),
        pumpState_(PUMP_OFF),
        pumpStartTime_(0) {}

  void begin() {
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, RELAY_OFF);  // Ensure pump starts OFF
  }

  /**
   * ENVIRONMENTAL SAFETY CHECK
   * Returns true if temperature and humidity are within operating bounds.
   * Prevents watering during extreme conditions that could harm plants.
   */
  bool isEnvSafe(float temperatureC, float humidityPercent) const {
    return temperatureC >= TEMP_MIN_C && temperatureC <= TEMP_MAX_C &&
           humidityPercent >= HUMIDITY_MIN_PERCENT;
  }

  bool inCooldown() const {
    if (lastWaterTime_ == 0) {
      return false;  // Never watered yet
    }
    return (millis() - lastWaterTime_) < PUMP_COOLDOWN_MS;
  }

  unsigned long cooldownRemainingSeconds() const {
    if (!inCooldown()) {
      return 0;
    }
    return (PUMP_COOLDOWN_MS - (millis() - lastWaterTime_)) / 1000UL;
  }

  /**
   * EVALUATE WATERING DECISION
   * 
   * Called every sensor read cycle (2 seconds).
   * Returns decision struct with all check results for telemetry.
   * 
   * DEBOUNCE LOGIC:
   * Soil must read "dry" for 3 consecutive readings (6 seconds)
   * before triggering watering. This filters transient spikes
   * from temperature drift or probe oxidation.
   */
  IrrigationDecision evaluate(const SensorData& data) {
    IrrigationDecision decision{};
    decision.shouldWater = false;
    decision.soilIsDry = false;
    decision.envIsSafe = false;
    decision.tankHasWater = false;
    decision.cooldownExpired = false;
    decision.reason = "Sensor read failed";

    // Fail-safe: Don't water if sensor data is invalid
    if (!data.validReading) {
      return decision;
    }

    // Evaluate all 5 conditions independently
    decision.soilIsDry = data.soilMoisturePercent < SOIL_DRY_THRESHOLD_PERCENT;
    decision.envIsSafe = isEnvSafe(data.temperatureC, data.humidityPercent);
    decision.tankHasWater = data.tankHasWater;
    decision.cooldownExpired = !inCooldown();

    // Debounce: Track consecutive dry readings
    if (decision.soilIsDry) {
      if (consecutiveDryReadings_ < 255) {
        consecutiveDryReadings_++;
      }
    } else {
      consecutiveDryReadings_ = 0;  // Reset on wet reading
    }

    const bool debouncePassed = consecutiveDryReadings_ >= DECISION_DEBOUNCE_READINGS;

    // Final decision: ALL conditions must be true
    decision.shouldWater = decision.soilIsDry && decision.envIsSafe &&
                           decision.tankHasWater && decision.cooldownExpired &&
                           debouncePassed;

    // Set human-readable reason (priority order for diagnostics)
    if (decision.shouldWater) {
      decision.reason = "Ready to water";
    } else if (!decision.soilIsDry) {
      decision.reason = "Soil moisture is fine";
    } else if (!debouncePassed) {
      decision.reason = "Waiting for stable dry reading";
    } else if (!decision.tankHasWater) {
      decision.reason = "Tank is empty";
    } else if (!decision.envIsSafe) {
      decision.reason = "Temperature/humidity out of range";
    } else if (!decision.cooldownExpired) {
      decision.reason = "Cooldown is active";
    } else {
      decision.reason = "Monitoring";
    }

    return decision;
  }

  /**
   * START PUMP
   * Returns false if pump is already running (safety interlock).
   */
  bool startPump() {
    if (pumpState_ != PUMP_OFF) {
      return false;
    }

    digitalWrite(PIN_RELAY, RELAY_ON);
    pumpState_ = PUMP_RUNNING;
    pumpStartTime_ = millis();
    return true;
  }

  void stopPump() {
    digitalWrite(PIN_RELAY, RELAY_OFF);
    pumpState_ = PUMP_OFF;
    pumpStartTime_ = 0;
  }

  /**
   * EMERGENCY STOP
   * Called by watchdog when pump exceeds maximum runtime.
   * Sets state to WATCHDOG_TRIGGERED for error handling.
   */
  void emergencyStop() {
    stopPump();
    pumpState_ = PUMP_WATCHDOG_TRIGGERED;
  }

  /**
   * UPDATE (called every loop iteration)
   * 
   * Checks pump runtime and triggers safety stops:
   * 1. Normal stop: After PUMP_ON_DURATION_MS (5 seconds)
   * 2. Emergency stop: After PUMP_WATCHDOG_MS (10 seconds)
   * 
   * Watchdog provides protection if relay fails to respond.
   */
  void update() {
    if (pumpState_ != PUMP_RUNNING) {
      return;
    }

    const unsigned long elapsed = millis() - pumpStartTime_;
    
    // Watchdog: Emergency stop if runtime exceeds 2× normal
    if (elapsed >= PUMP_WATCHDOG_MS) {
      emergencyStop();
      return;
    }

    // Normal stop: Duration complete
    if (elapsed >= PUMP_ON_DURATION_MS) {
      stopPump();
      lastWaterTime_ = millis();      // Start cooldown timer
      consecutiveDryReadings_ = 0;    // Reset debounce for next cycle
    }
  }

  PumpState state() const {
    return pumpState_;
  }

  bool isRunning() const {
    return pumpState_ == PUMP_RUNNING;
  }

  unsigned long pumpRuntimeSeconds() const {
    if (!isRunning()) {
      return 0;
    }
    return (millis() - pumpStartTime_) / 1000UL;
  }
};

#endif
