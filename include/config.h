#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

constexpr uint8_t PIN_SOIL_SENSOR = 34;
constexpr uint8_t PIN_DHT = 4;
constexpr uint8_t PIN_TANK_SWITCH = 5;
constexpr uint8_t PIN_RELAY = 18;

constexpr uint8_t DHT_SENSOR_TYPE = DHT11;

constexpr int SOIL_DRY_THRESHOLD_PERCENT = 60;
constexpr float TEMP_MIN_C = 15.0f;
constexpr float TEMP_MAX_C = 40.0f;
constexpr float HUMIDITY_MIN_PERCENT = 30.0f;

constexpr uint8_t DECISION_DEBOUNCE_READINGS = 3;

constexpr unsigned long SENSOR_READ_INTERVAL_MS = 2000UL;
constexpr unsigned long PUMP_ON_DURATION_MS = 5000UL;
constexpr unsigned long PUMP_COOLDOWN_MS = 60000UL;
constexpr unsigned long PUMP_WATCHDOG_MS = 10000UL;
constexpr unsigned long TELEMETRY_INTERVAL_MS = 3000UL;

constexpr int SERIAL_BAUD_RATE = 115200;

constexpr uint8_t RELAY_ON = HIGH;
constexpr uint8_t RELAY_OFF = LOW;

enum SystemState {
  STATE_MONITORING = 0,
  STATE_WATERING = 1,
  STATE_COOLDOWN = 2,
  STATE_ERROR = 3
};

#endif
