#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"
#include "calibration.h"
#include "sensor_data.h"

class Sensors {
 private:
  DHT dht_;
  int dhtErrorCount_;
  bool lastTankState_;
  bool lastRawTankState_;
  unsigned long tankDebounceStart_;

  // Production calibration references for the capacitive soil sensor.
  // Values are now fetched from NVS via Calibration class.
  static constexpr unsigned long TANK_DEBOUNCE_MS = 50UL;

 public:
  Sensors()
      : dht_(PIN_DHT, DHT_SENSOR_TYPE),
        dhtErrorCount_(0),
        lastTankState_(false),
        lastRawTankState_(false),
        tankDebounceStart_(0) {}

  void begin() {
    analogReadResolution(12);

    pinMode(PIN_TANK_SWITCH, TANK_SWITCH_PIN_MODE);

    const bool initialTankState = digitalRead(PIN_TANK_SWITCH) == TANK_WATER_PRESENT_LEVEL;
    lastTankState_ = initialTankState;
    lastRawTankState_ = initialTankState;
    tankDebounceStart_ = millis();

    dht_.begin();

    // Allow DHT startup stabilization.
    delay(1500);
  }

  void update() {
    // Debounce float-switch transitions to suppress slosh noise.
    const bool currentTankState = digitalRead(PIN_TANK_SWITCH) == TANK_WATER_PRESENT_LEVEL;
    if (currentTankState != lastRawTankState_) {
      lastRawTankState_ = currentTankState;
      tankDebounceStart_ = millis();
    }
    if ((millis() - tankDebounceStart_) > TANK_DEBOUNCE_MS && lastTankState_ != lastRawTankState_) {
      lastTankState_ = lastRawTankState_;
    }
  }

  SensorData read() {
    SensorData data{};
    data.validReading = true;

    // Convert raw ADC to moisture percentage using calibrated dry/wet points.
    const int rawSoil = analogRead(PIN_SOIL_SENSOR);
    data.soilMoisturePercent = static_cast<float>(map(rawSoil, calibration.getSoilRawDry(), calibration.getSoilRawWet(), 0, 100));
    data.soilMoisturePercent = constrain(data.soilMoisturePercent, 0.0f, 100.0f);

    data.temperatureC = dht_.readTemperature();
    data.humidityPercent = dht_.readHumidity();

    if (isnan(data.temperatureC) || isnan(data.humidityPercent)) {
      data.validReading = false;
      dhtErrorCount_++;
    } else {
      dhtErrorCount_ = 0;
    }

    data.tankHasWater = lastTankState_;

    return data;
  }

  bool healthy() const {
    return dhtErrorCount_ < 3;  // Allow up to two transient failures before error handling.
  }
};

#endif
