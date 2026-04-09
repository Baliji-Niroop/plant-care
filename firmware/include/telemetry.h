#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "irrigation.h"

class Telemetry {
 private:
  unsigned long lastReportMs_;

 public:
  Telemetry() : lastReportMs_(0) {}

  void begin() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(100);
    Serial.println();
    Serial.println("ESP32 Smart Plant Irrigation");
    Serial.println("System ready. Monitoring started.");
    Serial.println();
  }

  void printSensorData(const SensorData& data) {
    Serial.println("--- Sensor Snapshot ---");
    Serial.print("Soil: ");
    Serial.print(data.soilMoisturePercent, 1);
    Serial.println(data.soilMoisturePercent < SOIL_DRY_THRESHOLD_PERCENT ? "% [dry]" : "% [ok]");

    Serial.print("Temp: ");
    Serial.print(data.temperatureC, 1);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(data.humidityPercent, 1);
    Serial.println(" %");

    Serial.print("Tank: ");
    Serial.println(data.tankHasWater ? "available" : "empty");
    Serial.println();
  }

  void printDecision(const IrrigationDecision& decision) {
    Serial.println("--- Decision ---");
    Serial.print("Action: ");
    Serial.println(decision.shouldWater ? "WATER NOW" : "HOLD");
    Serial.print("Reason: ");
    Serial.println(decision.reason);
    Serial.println();
  }

  void printSystemState(SystemState state, const IrrigationController& controller) {
    Serial.print("Mode: ");
    switch (state) {
      case STATE_MONITORING:
        Serial.println("monitoring");
        break;
      case STATE_WATERING:
        Serial.println("watering");
        break;
      case STATE_COOLDOWN:
        Serial.println("cooldown");
        break;
      case STATE_ERROR:
        Serial.println("error");
        break;
    }

    Serial.print("Pump: ");
    switch (controller.state()) {
      case PUMP_OFF:
        Serial.println("off");
        break;
      case PUMP_RUNNING:
        Serial.print("running (");
        Serial.print(controller.pumpRuntimeSeconds());
        Serial.println("s)");
        break;
      case PUMP_WATCHDOG_TRIGGERED:
        Serial.println("watchdog lock");
        break;
    }
    Serial.println();
  }

  void periodicReport(const SensorData& data,
                      const IrrigationDecision& decision,
                      SystemState state,
                      const IrrigationController& controller) {
    if (millis() - lastReportMs_ < TELEMETRY_INTERVAL_MS) {
      return;
    }

    Serial.println("========== SYSTEM STATUS ==========");
    Serial.print("Uptime: ");
    Serial.print(millis() / 1000UL);
    Serial.println("s");
    printSensorData(data);
    printDecision(decision);
    printSystemState(state, controller);

    lastReportMs_ = millis();
  }
};

#endif
