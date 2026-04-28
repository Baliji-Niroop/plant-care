/**
 * @file wifi_telemetry.h
 * @brief MQTT and Wi-Fi telemetry reporting for remote plant monitoring.
 *
 * Integrates an optional Wi-Fi connection and PubSubClient to report
 * SensorData frames and accept remote overrides via MQTT.
 */
#ifndef WIFI_TELEMETRY_H
#define WIFI_TELEMETRY_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "sensor_data.h"

class WifiTelemetry {
 private:
  WiFiClient espClient_;
  PubSubClient client_;
  unsigned long lastPublishTime_;
  const unsigned long publishIntervalMs_;
  bool mqttEnabled_;

  void connectWiFi() {
    Serial.printf("[WiFi] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      delay(500);
      Serial.print(".");
      retries++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n[WiFi] Connected successfully");
    } else {
      Serial.println("\n[WiFi] Connection failed, continuing offline");
    }
  }

  void connectMQTT() {
    if (!client_.connected() && WiFi.status() == WL_CONNECTED) {
      Serial.print("[MQTT] Connecting to broker...");
      
      String clientId = "ESP32PlantCare-";
      clientId += String(random(0xffff), HEX);
      
      if (client_.connect(clientId.c_str())) {
        Serial.println("connected");
        client_.subscribe("/plant/override");
      } else {
        Serial.printf("failed, rc=%d. Will retry later.\n", client_.state());
      }
    }
  }

  static void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    message.reserve(length);
    for (unsigned int i = 0; i < length; i++) {
      message += static_cast<char>(payload[i]);
    }
    
    Serial.printf("[MQTT] Message arrived on [%s]: %s\n", topic, message.c_str());
    
    // Remote override logic (future expansion)
    if (String(topic) == "/plant/override" && message == "PUMP_ON") {
      Serial.println("[MQTT] Manual override: PUMP_ON requested via remote telemetry");
    }
  }

 public:
  /**
   * @brief Construct a new Wifi Telemetry object
   */
  WifiTelemetry() 
      : client_(espClient_), 
        lastPublishTime_(0), 
        publishIntervalMs_(30000UL), 
        mqttEnabled_(false) {
    if (String(WIFI_SSID) != "YOUR_WIFI_SSID") {
      mqttEnabled_ = true;
    }
  }

  /**
   * @brief Connects to Wi-Fi and configures the MQTT client.
   */
  void setup() {
    if (!mqttEnabled_) {
      Serial.println("[WiFi] Telemetry disabled (default SSID detected).");
      return;
    }
    
    connectWiFi();
    client_.setServer(MQTT_BROKER, MQTT_PORT);
    client_.setCallback(mqttCallback);
  }

  /**
   * @brief Maintains the MQTT connection and processes incoming messages.
   * Call frequently in loop().
   */
  void update() {
    if (!mqttEnabled_) return;
    
    if (WiFi.status() == WL_CONNECTED) {
      if (!client_.connected()) {
        connectMQTT();
      } else {
        client_.loop();
      }
    }
  }

  /**
   * @brief Publishes a JSON payload containing the current plant status to the MQTT broker.
   * Rate limited to publishIntervalMs_.
   * 
   * @param data The most recent SensorData frame.
   * @param state The current system state machine state.
   * @param decision The string explanation of the current decision.
   */
  void publishData(const SensorData& data, SystemState state, const char* decision) {
    if (!mqttEnabled_ || !client_.connected()) return;

    if (millis() - lastPublishTime_ >= publishIntervalMs_) {
      lastPublishTime_ = millis();
      
      String json = "{";
      json += "\"temp_c\":" + (isnan(data.temperatureC) ? "null" : String(data.temperatureC, 1)) + ",";
      json += "\"humidity_pct\":" + (isnan(data.humidityPercent) ? "null" : String(data.humidityPercent, 1)) + ",";
      json += "\"soil_moisture_pct\":" + String(data.soilMoisturePercent) + ",";
      json += "\"tank_ok\":" + String(data.tankHasWater ? "true" : "false") + ",";
      json += "\"sensor_valid\":" + String(data.validReading ? "true" : "false") + ",";
      json += "\"state\":" + String(state) + ",";
      json += "\"decision\":\"" + String(decision) + "\"";
      json += "}";

      client_.publish("/plant/telemetry", json.c_str());
    }
  }
};

// Global instance defined in the main compilation unit (firmware.ino).
extern WifiTelemetry wifiTelemetry;

#endif // WIFI_TELEMETRY_H
