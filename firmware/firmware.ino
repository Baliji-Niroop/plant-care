// ESP32 smart plant care production firmware.
// Coordinates sensing, irrigation decisions, safety checks, and telemetry.

#include "include/config.h"
#include "include/sensors.h"
#include "include/irrigation.h"
#include "include/telemetry.h"
#include "include/wifi_telemetry.h"
#include "include/calibration.h"

Sensors sensors;
IrrigationController irrigation;
Telemetry telemetry;
WifiTelemetry wifiTelemetry;
Calibration calibration(PIN_SOIL_SENSOR);

SystemState systemState = STATE_MONITORING;
SystemState previousState = STATE_MONITORING;
unsigned long lastSensorReadMs = 0;
SensorData currentData{};
IrrigationDecision lastDecision{};
constexpr unsigned long MAIN_LOOP_DELAY_MS = 100UL;

void setSystemState(SystemState nextState) {
  if (systemState == nextState) {
    return;
  }

  previousState = systemState;
  systemState = nextState;

  Serial.print("[state] ");
  Serial.print(previousState);
  Serial.print(" -> ");
  Serial.println(systemState);
}

void setup() {
  telemetry.begin();
  calibration.setup();
  sensors.begin();
  irrigation.begin();
  wifiTelemetry.setup();

  Serial.println("[init] Production firmware online");
}

void loop() {
  // Keep pump safety enforcement active on every loop.
  irrigation.update();

  // Keep sensors updated for proper fast-debouncing
  sensors.update();

  wifiTelemetry.update();

  // Read and evaluate sensors at a fixed interval.
  const bool dueForRead = (millis() - lastSensorReadMs) >= SENSOR_READ_INTERVAL_MS;
  if (dueForRead) {
    currentData = sensors.read();
    lastSensorReadMs = millis();
    lastDecision = irrigation.evaluate(currentData);

    // Transition to error only after repeated sensor-health failures.
    if (!sensors.healthy()) {
      setSystemState(STATE_ERROR);
    }
  }

  switch (systemState) {
    case STATE_MONITORING:
      if (lastDecision.shouldWater) {
        const bool started = irrigation.startPump();
        if (started) {
          setSystemState(STATE_WATERING);
          Serial.println("[event] Pump started");
        }
      }
      break;

    case STATE_WATERING:
      if (!irrigation.isRunning()) {
        setSystemState(STATE_COOLDOWN);
        Serial.println("[event] Pump stopped");
      }
      break;

    case STATE_COOLDOWN:
      if (irrigation.state() == PUMP_WATCHDOG_TRIGGERED) {
        setSystemState(STATE_ERROR);
      } else if (!irrigation.inCooldown()) {
        setSystemState(STATE_MONITORING);
      }
      break;

    case STATE_ERROR:
      if (irrigation.isRunning()) {
        irrigation.stopPump();
        Serial.println("[safety] Pump forced off in error state");
      }
      if (irrigation.state() == PUMP_WATCHDOG_TRIGGERED) {
        Serial.println("[fault] Watchdog lock active; reset controller to re-arm pump");
      } else if (sensors.healthy()) {
        Serial.println("[info] sensors ok again");
        setSystemState(STATE_MONITORING);
      }
      break;
  }

  telemetry.periodicReport(currentData, lastDecision, systemState, irrigation);
  wifiTelemetry.publishData(currentData, systemState, lastDecision.reason.c_str());

  delay(MAIN_LOOP_DELAY_MS);
}
