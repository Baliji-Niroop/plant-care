#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"

/**
 * SENSOR DATA STRUCTURE
 * Holds readings from all sensors for decision-making.
 * validReading flag indicates if DHT11 data should be trusted.
 */
struct SensorData {
  float soilMoisturePercent;  // 0-100%, mapped from ADC
  float temperatureC;         // Celsius from DHT11
  float humidityPercent;      // Relative humidity from DHT11
  bool tankHasWater;          // Float switch state (debounced)
  bool validReading;          // False if DHT11 returned NaN
};

/**
 * SENSORS CLASS
 * Abstracts hardware interaction for all input sensors.
 * Handles ADC conversion, DHT timing, and switch debouncing.
 */
class Sensors {
 private:
  DHT dht_;
  int dhtErrorCount_;           // Consecutive DHT failures
  bool lastTankState_;          // For debounce comparison
  unsigned long tankDebounceStart_;

 public:
  Sensors()
      : dht_(PIN_DHT, DHT_SENSOR_TYPE),
        dhtErrorCount_(0),
        lastTankState_(false),
        tankDebounceStart_(0) {}

  void begin() {
    // ESP32 ADC resolution: 12-bit (0-4095)
    analogReadResolution(12);
    
    // Tank switch uses internal pull-up (saves external resistor)
    pinMode(PIN_TANK_SWITCH, INPUT_PULLUP);
    
    dht_.begin();
    
    // DHT11 needs 1.5sec warm-up for stable readings
    // Increase to 2sec in cold environments (<10°C)
    delay(1500);
  }

  /**
   * READ ALL SENSORS
   * Returns complete sensor snapshot for decision logic.
   * 
   * SOIL SENSOR CALIBRATION:
   * - Raw ADC: 4095 (dry/air) → 0 (submerged in water)
   * - map() inverts and scales to 0-100%
   * - If your sensor differs, calibrate per CALIBRATION.md
   */
  SensorData read() {
    SensorData data{};
    data.validReading = true;

    // Soil moisture: ADC reading inverted to percentage
    const int rawSoil = analogRead(PIN_SOIL_SENSOR);
    data.soilMoisturePercent = static_cast<float>(map(rawSoil, 4095, 0, 0, 100));
    data.soilMoisturePercent = constrain(data.soilMoisturePercent, 0.0f, 100.0f);

    // DHT11 temperature and humidity
    data.temperatureC = dht_.readTemperature();
    data.humidityPercent = dht_.readHumidity();

    /**
     * DHT ERROR HANDLING (3-strike rule):
     * - Single NaN: Use last valid reading (sensor glitch)
     * - 3 consecutive NaN: Trigger ERROR state (hardware issue)
     * 
     * Common causes: timing violations, EMI from pump, low voltage
     */
    if (isnan(data.temperatureC) || isnan(data.humidityPercent)) {
      data.validReading = false;
      dhtErrorCount_++;
    } else {
      dhtErrorCount_ = 0;
    }

    // Tank float switch with 50ms debounce
    // Prevents false readings from water slosh or switch bounce
    const bool currentTankState = digitalRead(PIN_TANK_SWITCH) == HIGH;
    if (currentTankState != lastTankState_) {
      tankDebounceStart_ = millis();
    }
    if (millis() - tankDebounceStart_ > 50UL) {
      lastTankState_ = currentTankState;
    }
    data.tankHasWater = lastTankState_;

    return data;
  }

  /**
   * SENSOR HEALTH CHECK
   * Returns false after 3 consecutive DHT failures.
   * Used by main loop to enter ERROR state.
   */
  bool healthy() const {
    return dhtErrorCount_ < 3;
  }
};

#endif
