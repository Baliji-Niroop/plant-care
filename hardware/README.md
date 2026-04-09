# Hardware Build Status

## Current Phase

Component procurement and staged assembly planning.

## Component Checklist

- [x] ESP32 DevKit V1
- [x] Capacitive soil moisture sensor
- [x] DHT22 temperature and humidity sensor
- [x] 1-channel relay module
- [ ] 5V mini water pump
- [ ] Tank level switch or float switch
- [ ] External power module and wiring accessories

## Build Stages

### Stage 1: Bench Wiring

- ESP32, sensors, and relay connected on breadboard
- Pump switching validated with safe low-voltage test
- Serial telemetry verified for all sensor reads

Photo placeholder:
- docs/images/prototype-stage1-placeholder.png

### Stage 2: Integrated Prototype

- Tank line and pump mounted
- Tank-low interlock validated
- Cooldown and watchdog safety validated in physical setup

Photo placeholder:
- docs/images/prototype-stage2-placeholder.png

## Known Risks and Mitigations

- Pump inrush current: use separate supply and common ground
- Relay noise or chatter: keep cooldown and watchdog enabled
- Sensor drift: recalibrate dry and wet points before final demo

## Calibration Log

Record measured values from physical testing before final tuning.

- Soil raw (air dry): pending
- Soil raw (water saturated): pending
- Final dry threshold percent: pending
- Final pump duration ms: pending

## Next Steps

1. Complete Stage 1 wiring and capture photos.
2. Replace stage placeholders with real hardware images.
3. Run full validation checklist and attach evidence images.
4. Update firmware thresholds using measured calibration data.
