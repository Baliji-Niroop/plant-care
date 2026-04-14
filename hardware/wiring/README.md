# Hardware Wiring and Assembly

This is where you figure out how to plug everything in. The wiring itself is straightforward—four sensors and one output relay, each connected to a specific GPIO pin.

## What's Here

**[build-guide.md](./build-guide.md)** — The actual assembly procedure. Step-by-step: which wire goes where, how to check that each connection is correct, how to validate everything before flashing firmware.

**[wiring-diagram.mmd](./wiring-diagram.mmd)** — The circuit diagram in text format (Mermaid). Shows all the connections visually.

## Pin Map (Quick Reference)

This is where each sensor plugs in:

- **GPIO34** (analog input) — Soil moisture sensor ADC output
- **GPIO4** (digital I/O) — DHT22 data line
- **GPIO5** (digital input) — Float switch (LOW = water present)
- **GPIO18** (digital output) — Relay module control
- **5V external supply** — Powers the relay and pump (separate from ESP32 power to avoid brownout)

## Before You Start Wiring

1. Lay out all components and verify you have everything from `../components/README.md`
2. Look at the wiring diagram in `wiring-diagram.mmd` to understand the big picture
3. Check your relay module to see if it's active-high or active-low—you'll need to tell the firmware
4. Make sure your 5V power adapter is rated for at least 2A (pump inrush current is real)

## Assembly Path

1. Read through `build-guide.md` completely before touching anything
2. Wire everything following the step-by-step procedure
3. Validate each connection doesn't have shorts or reversed polarity
4. Run the verification tests to make sure all signals actually work
5. Flash the firmware (see `../../firmware/README.md`)
6. Calibrate and deploy

If something doesn't work during assembly, the troubleshooting section in `build-guide.md` walks through common issues.
