Assembly Notes
==============

Purpose:
- Short engineering log of key prototype milestones.

Day 1:
- Verified ESP32 upload path using a basic blink sketch.
- Observed initial DHT `NaN` values.
- Increased startup warm-up delay to stabilize readings.

Day 2:
- Connected soil sensor output to GPIO34.
- Recorded soil calibration references (dry approximately 3950, wet approximately 1650).
- Connected tank switch to GPIO5 with `INPUT_PULLUP` behavior.

Day 3:
- Wired relay control input to GPIO18.
- Powered relay module from 5V.
- Performed pump timing check with a simple measured output test.

Day 4:
- Completed integrated control logic validation.
- Confirmed dry soil triggers watering.
- Confirmed cooldown blocks immediate retrigger.
- Retained watchdog protection after observing one relay-run anomaly.

Current status:
- Core control behavior is stable in simulation and prepared for full bench validation.
