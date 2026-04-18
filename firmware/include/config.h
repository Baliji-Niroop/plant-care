#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Production firmware pin map (ESP32 DevKit V1)
constexpr uint8_t PIN_SOIL_SENSOR = 34;   // Analog input, ADC1_CH6
constexpr uint8_t PIN_DHT = 4;            // DHT data
constexpr uint8_t PIN_TANK_SWITCH = 5;    // Float switch
constexpr uint8_t PIN_RELAY = 18;         // Relay input

// Kept numeric to avoid include-order dependency on DHT headers.
constexpr uint8_t DHT_SENSOR_TYPE = 22;

// Decision thresholds
constexpr int SOIL_DRY_THRESHOLD_PERCENT = 60;

constexpr float TEMP_MIN_C = 15.0f;
constexpr float TEMP_MAX_C = 40.0f;
constexpr float HUMIDITY_MIN_PERCENT = 30.0f;
constexpr float HUMIDITY_MAX_PERCENT = 80.0f;

// Require several dry readings to filter noise.
constexpr uint8_t DECISION_DEBOUNCE_READINGS = 3;

// Timing constants
constexpr unsigned long SENSOR_READ_INTERVAL_MS = 2000UL;
constexpr unsigned long PUMP_ON_DURATION_MS = 5000UL;
constexpr unsigned long PUMP_COOLDOWN_MS = 60000UL;
constexpr unsigned long PUMP_WATCHDOG_MS = 10000UL;
constexpr unsigned long TELEMETRY_INTERVAL_MS = 3000UL;

constexpr int SERIAL_BAUD_RATE = 115200;

// Relay polarity for physical hardware. Swap if your relay is active-low.
constexpr uint8_t RELAY_ON = HIGH;
constexpr uint8_t RELAY_OFF = LOW;

// Tank switch electrical polarity.
// Default: INPUT_PULLUP with switch wired between GPIO and GND.
// In this default wiring, LOW means water is present.
constexpr uint8_t TANK_SWITCH_PIN_MODE = INPUT_PULLUP;
constexpr uint8_t TANK_WATER_PRESENT_LEVEL = LOW;

enum SystemState {
  STATE_MONITORING = 0,
  STATE_WATERING = 1,
  STATE_COOLDOWN = 2,
  STATE_ERROR = 3
};

#endif
