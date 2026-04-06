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
  irrigation.update();

  const bool dueForRead = (millis() - lastSensorReadMs) >= SENSOR_READ_INTERVAL_MS;
  if (dueForRead) {
    currentData = sensors.read();
    lastSensorReadMs = millis();
    lastDecision = irrigation.evaluate(currentData);
  }

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
  delay(100);
}
