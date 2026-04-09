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

**From Initial Bench Testing (Day 2 - See assembly_notes.txt)**:
- Soil raw (air dry): 3950 ADC
- Soil raw (water saturated): 1650 ADC
- Current dry threshold: 60% (firmware/include/config.h - SOIL_DRY_THRESHOLD_PERCENT)
- Current pump duration: 5000 ms (firmware/include/config.h - PUMP_ON_DURATION_MS)

**Calibration Status**: ⚠️ **AWAITING PHYSICAL HARDWARE BUILD**
- [ ] Stage 1 bench assembly complete - recalibrate with actual components
- [ ] Final threshold tuning after observing real plant moisture patterns
- [ ] Pump timing optimization based on tank capacity and plant needs

## Next Steps

**Immediate (Blocking Hardware Build)**:
1. ✅ Procure missing components: 5V mini water pump, tank-level switch/float switch
2. Execute Stage 1 bench wiring using `hardware/BUILD_GUIDE.md` steps
3. Run physical validation checklist and capture evidence photos
4. Update `docs/images/prototype-stage1-placeholder.png` with real bench photo

**After Stage 1 Photos**:
5. Replace simulation results with hardware results in IEEE paper
6. Execute Stage 2 assembly and integrated testing
7. Update `docs/images/prototype-stage2-placeholder.png` with real assembly photo
8. Finalize calibration data from extended operation
