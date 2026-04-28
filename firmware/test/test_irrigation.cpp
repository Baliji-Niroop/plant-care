/**
 * @file test_irrigation.cpp
 * @brief Standalone unit tests for IrrigationController decision logic.
 *
 * Validates the 7-stage safety checks required before watering.
 * This can be run natively (e.g., via PlatformIO native env or g++).
 */

#include <iostream>
#include <cassert>
#include <string>

// --- Arduino Core Mocks ---
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define String std::string

unsigned long mock_millis_val = 0;
unsigned long millis() {
    return mock_millis_val;
}
void pinMode(uint8_t pin, uint8_t mode) {}
void digitalWrite(uint8_t pin, uint8_t val) {}

// --- System Includes ---
#include "../include/config.h"
#include "../include/sensor_data.h"
#include "../include/irrigation.h"

// --- Test Framework ---

class IrrigationTest {
 private:
  IrrigationController controller_;
  SensorData data_;

  IrrigationDecision evaluateWithDebounce() {
    // Run enough cycles to clear the debounce threshold
    for (int i = 0; i < DECISION_DEBOUNCE_READINGS; i++) {
      controller_.evaluate(data_);
    }
    return controller_.evaluate(data_);
  }

  void resetToPassConditions() {
    data_.validReading = true;
    data_.soilMoisturePercent = SOIL_DRY_THRESHOLD_PERCENT - 10.0f; // Dry
    data_.temperatureC = (TEMP_MAX_C + TEMP_MIN_C) / 2.0f;          // Safe
    data_.humidityPercent = (HUMIDITY_MAX_PERCENT + HUMIDITY_MIN_PERCENT) / 2.0f; // Safe
    data_.tankHasWater = true;                                      // Safe
    mock_millis_val = PUMP_COOLDOWN_MS + 1000;                      // Cooldown expired
  }

  void assertDecision(const std::string& testName, bool expectedWateringStatus) {
    const IrrigationDecision decision = evaluateWithDebounce();
    if (decision.shouldWater == expectedWateringStatus) {
      std::cout << "[PASS] " << testName << std::endl;
    } else {
      std::cerr << "[FAIL] " << testName << " - Expected: " << expectedWateringStatus 
                << ", Got: " << decision.shouldWater << " (" << decision.reason << ")" << std::endl;
      assert(false);
    }
  }

 public:
  void runAll() {
    std::cout << "=========================================\n";
    std::cout << " Running Irrigation Controller Tests\n";
    std::cout << "=========================================\n";

    // 1. Invalid Reading
    resetToPassConditions();
    data_.validReading = false;
    assertDecision("Stage 1: Invalid Sensor Frame", false);

    // 2. Soil Wet
    resetToPassConditions();
    data_.soilMoisturePercent = SOIL_DRY_THRESHOLD_PERCENT + 10.0f;
    assertDecision("Stage 2: Soil is Wet", false);

    // 3. Temperature Unsafe (Cold)
    resetToPassConditions();
    data_.temperatureC = TEMP_MIN_C - 5.0f;
    assertDecision("Stage 4: Temperature Too Cold", false);

    // 4. Humidity Unsafe (Dry)
    resetToPassConditions();
    data_.humidityPercent = HUMIDITY_MIN_PERCENT - 10.0f;
    assertDecision("Stage 5: Humidity Too Low", false);

    // 5. Tank Empty
    resetToPassConditions();
    data_.tankHasWater = false;
    assertDecision("Stage 6: Tank Empty Interlock", false);

    // 6. Cooldown Active
    resetToPassConditions();
    mock_millis_val = 0; 
    controller_.startPump();
    mock_millis_val += PUMP_ON_DURATION_MS;
    controller_.update(); // Stops pump, starts cooldown
    assertDecision("Stage 7: Cooldown Active", false);

    // 7. All Conditions Met (Happy Path)
    resetToPassConditions();
    assertDecision("Happy Path: All Checks Pass", true);

    std::cout << "=========================================\n";
    std::cout << " All tests passed successfully!\n";
    std::cout << "=========================================\n";
  }
};

int main() {
  IrrigationTest testSuite;
  testSuite.runAll();
  return 0;
}
