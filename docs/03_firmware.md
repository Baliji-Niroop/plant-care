# Firmware Overview

The firmware architecture is decoupled and strictly object-oriented. For a deep dive into the code implementation, refer to the [Firmware Directory](../firmware/).

## Build Systems
The project strictly supports:
1. **PlatformIO**: Configurations are fully defined in `firmware/platformio.ini`.
2. **Arduino CLI**: Standard compilation directly against `firmware/firmware.ino`.

## Module Segregation
- **Sensor Data (`sensor_data.h`)**: Pure data structures detached from hardware logic.
- **Decision Engine (`irrigation.h`)**: A standalone, testable logic class.
- **Hardware Abstraction (`sensors.h`)**: Translates physical GPIO states to SensorData frames.
- **Telemetry (`wifi_telemetry.h`)**: Pushes serialized status frames over MQTT.
