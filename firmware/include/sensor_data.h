#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

struct SensorData {
  float soilMoisturePercent;  // 0-100%
  float temperatureC;
  float humidityPercent;
  bool tankHasWater;
  bool validReading;
};

#endif // SENSOR_DATA_H
