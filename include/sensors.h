#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"

struct SensorData {
  float soilMoisturePercent;
  float temperatureC;
  float humidityPercent;
  bool tankHasWater;
  bool validReading;
};

class Sensors {
 private:
  DHT dht_;
  int dhtErrorCount_;
  bool lastTankState_;
  unsigned long tankDebounceStart_;

 public:
  Sensors()
      : dht_(PIN_DHT, DHT_SENSOR_TYPE),
        dhtErrorCount_(0),
        lastTankState_(false),
        tankDebounceStart_(0) {}

  void begin() {
    analogReadResolution(12);
    pinMode(PIN_TANK_SWITCH, INPUT_PULLUP);
    dht_.begin();
    delay(1500);
  }

  SensorData read() {
    SensorData data{};
    data.validReading = true;

    const int rawSoil = analogRead(PIN_SOIL_SENSOR);
    data.soilMoisturePercent = static_cast<float>(map(rawSoil, 4095, 0, 0, 100));
    data.soilMoisturePercent = constrain(data.soilMoisturePercent, 0.0f, 100.0f);

    data.temperatureC = dht_.readTemperature();
    data.humidityPercent = dht_.readHumidity();

    if (isnan(data.temperatureC) || isnan(data.humidityPercent)) {
      data.validReading = false;
      dhtErrorCount_++;
    } else {
      dhtErrorCount_ = 0;
    }

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

  bool healthy() const {
    return dhtErrorCount_ < 3;
  }
};

#endif
