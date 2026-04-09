#include <Arduino.h>
#include <DHTesp.h>

// Pin map
static const uint8_t PIN_DHT = 4;
static const uint8_t PIN_SOIL = 34;
static const uint8_t PIN_TANK_SWITCH = 5;
static const uint8_t PIN_RELAY = 18;

// Control tuning
// Lower threshold (35%) for easier simulation demos vs production firmware (60%).
// This makes watering cycles happen faster while validating behavior.
static const int MOISTURE_THRESHOLD_PERCENT = 35;   // Start watering below this moisture level
static const unsigned long WATERING_DURATION_MS = 6000;
static const unsigned long WATERING_WATCHDOG_MS = 10000;
static const unsigned long COOLDOWN_MS = 15000;
static const unsigned long LOOP_INTERVAL_MS = 1000;

DHTesp dht;

bool pumpOn = false;
bool inCooldown = false;
unsigned long wateringStartedAt = 0;
unsigned long cooldownStartedAt = 0;
unsigned long lastLoopAt = 0;
String lastDecision = "boot";

// Sensor snapshot for one control cycle
struct SensorFrame {
  float temperatureC;
  float humidityPercent;
  int soilRaw;
  int moisturePercent;
  bool tankHasWater;
  bool dhtValid;
};

// Sensor readout
SensorFrame readSensors() {
  SensorFrame s{};

  TempAndHumidity dhtData = dht.getTempAndHumidity();
  s.temperatureC = dhtData.temperature;
  s.humidityPercent = dhtData.humidity;
  s.dhtValid = !(isnan(s.temperatureC) || isnan(s.humidityPercent));

  s.soilRaw = analogRead(PIN_SOIL);
  // Simplified 0-4095 mapping for quick demo testing.
  // Production firmware uses calibrated values 3950-1650 (see firmware/include/sensors.h).
  s.moisturePercent = map(s.soilRaw, 0, 4095, 0, 100);
  s.moisturePercent = constrain(s.moisturePercent, 0, 100);

  // Slide switch HIGH means water is available.
  s.tankHasWater = (digitalRead(PIN_TANK_SWITCH) == HIGH);

  return s;
}

// Pump control
void setPump(bool on) {
  pumpOn = on;
  // Wokwi relay module is active-low (opposite of modular firmware active-high default).
  // Production config uses RELAY_ON=HIGH, but Wokwi relay expects LOW=ON.
  digitalWrite(PIN_RELAY, on ? LOW : HIGH);
}

void startWatering(unsigned long now) {
  setPump(true);
  wateringStartedAt = now;
  lastDecision = "watering_start";
  Serial.println("ACTION: watering_start");
}

void stopWateringAndCooldown(unsigned long now) {
  setPump(false);
  inCooldown = true;
  cooldownStartedAt = now;
  lastDecision = "watering_stop_cooldown_start";
  Serial.println("ACTION: watering_stop -> cooldown_start");
}

// Decision logic and cooldown
void updateIrrigation(const SensorFrame &s, unsigned long now) {
  if (pumpOn) {
    // Safety timeout if the pump runs too long.
    if (now - wateringStartedAt >= WATERING_WATCHDOG_MS) {
      setPump(false);
      inCooldown = true;
      cooldownStartedAt = now;
      lastDecision = "watchdog_stop";
      Serial.println("ACTION: watchdog_stop -> cooldown_start");
      return;
    }

    // Planned stop after the watering duration.
    if (now - wateringStartedAt >= WATERING_DURATION_MS) {
      stopWateringAndCooldown(now);
    }
    return;
  }

  if (inCooldown) {
    if (now - cooldownStartedAt >= COOLDOWN_MS) {
      inCooldown = false;
      lastDecision = "cooldown_end";
      Serial.println("ACTION: cooldown_end");
    } else {
      lastDecision = "hold_cooldown";
      return;
    }
  }

  const bool isDry = s.moisturePercent < MOISTURE_THRESHOLD_PERCENT;
  if (!s.tankHasWater) {
    lastDecision = "hold_tank_empty";
    return;
  }

  if (isDry && s.tankHasWater) {
    startWatering(now);
  } else {
    lastDecision = "hold_soil_ok";
  }
}

// Telemetry output
void printTelemetry(const SensorFrame &s, unsigned long now) {
  Serial.print("t_ms=");
  Serial.print(now);

  Serial.print(" temp_c=");
  if (s.dhtValid) {
    Serial.print(s.temperatureC, 1);
  } else {
    Serial.print("nan");
  }

  Serial.print(" humidity_pct=");
  if (s.dhtValid) {
    Serial.print(s.humidityPercent, 1);
  } else {
    Serial.print("nan");
  }

  Serial.print(" soil_raw=");
  Serial.print(s.soilRaw);

  Serial.print(" moisture_pct=");
  Serial.print(s.moisturePercent);

  Serial.print(" tank=");
  Serial.print(s.tankHasWater ? "OK" : "EMPTY");

  Serial.print(" pump=");
  Serial.print(pumpOn ? "ON" : "OFF");

  Serial.print(" cooldown=");
  Serial.print(inCooldown ? "YES" : "NO");

  Serial.print(" decision=");
  Serial.println(lastDecision);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TANK_SWITCH, INPUT);
  pinMode(PIN_RELAY, OUTPUT);

  dht.setup(PIN_DHT, DHTesp::DHT22);

  setPump(false);

  Serial.println("=== ESP32 Plant Irrigation (Local Wokwi Single-File Build) ===");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastLoopAt < LOOP_INTERVAL_MS) {
    return;
  }
  lastLoopAt = now;

  SensorFrame sensors = readSensors();
  updateIrrigation(sensors, now);
  printTelemetry(sensors, now);
}
