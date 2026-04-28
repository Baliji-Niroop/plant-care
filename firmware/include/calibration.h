/**
 * @file calibration.h
 * @brief Manages non-volatile storage (NVS) for sensor calibration data.
 *
 * Provides a startup routine to dynamically set dry and wet references
 * without needing to reflash the firmware.
 */
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <Preferences.h>

class Calibration {
 private:
  Preferences preferences_;
  int soilRawDry_;
  int soilRawWet_;
  const uint8_t pinSoil_;

  static constexpr int DEFAULT_SOIL_RAW_DRY = 3950;
  static constexpr int DEFAULT_SOIL_RAW_WET = 1650;
  static constexpr unsigned long CALIBRATION_WINDOW_MS = 10000UL;

 public:
  /**
   * @brief Construct a new Calibration object
   * 
   * @param soilPin The GPIO pin used for analog soil moisture reading.
   */
  explicit Calibration(uint8_t soilPin) 
      : soilRawDry_(DEFAULT_SOIL_RAW_DRY), 
        soilRawWet_(DEFAULT_SOIL_RAW_WET), 
        pinSoil_(soilPin) {}

  /**
   * @brief Initializes NVS and handles the initial 10-second calibration window.
   */
  void setup() {
    preferences_.begin("plantcare", false);
    soilRawDry_ = preferences_.getInt("soilDry", DEFAULT_SOIL_RAW_DRY);
    soilRawWet_ = preferences_.getInt("soilWet", DEFAULT_SOIL_RAW_WET);

    Serial.println("=========================================");
    Serial.println(" Calibration Mode Active (10 seconds)    ");
    Serial.println("=========================================");
    Serial.println(" -> Send 'D' via Serial to save current reading as DRY reference.");
    Serial.println(" -> Send 'W' via Serial to save current reading as WET reference.");
    Serial.printf(" [Current Status] DRY: %d, WET: %d\n", soilRawDry_, soilRawWet_);
    
    const unsigned long startWait = millis();
    while (millis() - startWait < CALIBRATION_WINDOW_MS) {
      if (Serial.available() > 0) {
        const char c = static_cast<char>(Serial.read());
        if (c == 'D' || c == 'd') {
          soilRawDry_ = analogRead(pinSoil_);
          preferences_.putInt("soilDry", soilRawDry_);
          Serial.printf(" [Update] DRY reference set to: %d\n", soilRawDry_);
        } else if (c == 'W' || c == 'w') {
          soilRawWet_ = analogRead(pinSoil_);
          preferences_.putInt("soilWet", soilRawWet_);
          Serial.printf(" [Update] WET reference set to: %d\n", soilRawWet_);
        }
      }
      delay(10);
    }
    Serial.println("=========================================");
    Serial.println(" Calibration Mode Ended                  ");
    Serial.println("=========================================");
  }

  /**
   * @brief Get the raw ADC value representing dry soil.
   */
  int getSoilRawDry() const { 
    return soilRawDry_; 
  }

  /**
   * @brief Get the raw ADC value representing wet (saturated) soil.
   */
  int getSoilRawWet() const { 
    return soilRawWet_; 
  }
};

// Global instance defined in the main compilation unit (firmware.ino).
extern Calibration calibration;

#endif // CALIBRATION_H
