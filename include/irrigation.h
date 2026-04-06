#ifndef IRRIGATION_H
#define IRRIGATION_H

#include <Arduino.h>
#include "config.h"
#include "sensors.h"

enum PumpState {
  PUMP_OFF = 0,
  PUMP_RUNNING = 1,
  PUMP_WATCHDOG_TRIGGERED = 2
};

struct IrrigationDecision {
  bool shouldWater;
  bool soilIsDry;
  bool envIsSafe;
  bool tankHasWater;
  bool cooldownExpired;
  String reason;
};

class IrrigationController {
 private:
  unsigned long lastWaterTime_;
  uint8_t consecutiveDryReadings_;
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
    digitalWrite(PIN_RELAY, RELAY_OFF);
  }

  bool isEnvSafe(float temperatureC, float humidityPercent) const {
    return temperatureC >= TEMP_MIN_C && temperatureC <= TEMP_MAX_C &&
           humidityPercent >= HUMIDITY_MIN_PERCENT;
  }

  bool inCooldown() const {
    if (lastWaterTime_ == 0) {
      return false;
    }
    return (millis() - lastWaterTime_) < PUMP_COOLDOWN_MS;
  }

  unsigned long cooldownRemainingSeconds() const {
    if (!inCooldown()) {
      return 0;
    }
    return (PUMP_COOLDOWN_MS - (millis() - lastWaterTime_)) / 1000UL;
  }

  IrrigationDecision evaluate(const SensorData& data) {
    IrrigationDecision decision{};
    decision.shouldWater = false;
    decision.soilIsDry = false;
    decision.envIsSafe = false;
    decision.tankHasWater = false;
    decision.cooldownExpired = false;
    decision.reason = "Sensor read failed";

    if (!data.validReading) {
      return decision;
    }

    decision.soilIsDry = data.soilMoisturePercent < SOIL_DRY_THRESHOLD_PERCENT;
    decision.envIsSafe = isEnvSafe(data.temperatureC, data.humidityPercent);
    decision.tankHasWater = data.tankHasWater;
    decision.cooldownExpired = !inCooldown();

    if (decision.soilIsDry) {
      if (consecutiveDryReadings_ < 255) {
        consecutiveDryReadings_++;
      }
    } else {
      consecutiveDryReadings_ = 0;
    }

    const bool debouncePassed = consecutiveDryReadings_ >= DECISION_DEBOUNCE_READINGS;

    decision.shouldWater = decision.soilIsDry && decision.envIsSafe &&
                           decision.tankHasWater && decision.cooldownExpired &&
                           debouncePassed;

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

  void emergencyStop() {
    stopPump();
    pumpState_ = PUMP_WATCHDOG_TRIGGERED;
  }

  void update() {
    if (pumpState_ != PUMP_RUNNING) {
      return;
    }

    const unsigned long elapsed = millis() - pumpStartTime_;
    if (elapsed >= PUMP_WATCHDOG_MS) {
      emergencyStop();
      return;
    }

    if (elapsed >= PUMP_ON_DURATION_MS) {
      stopPump();
      lastWaterTime_ = millis();
      consecutiveDryReadings_ = 0;
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
