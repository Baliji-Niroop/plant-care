# Hardware Components

This folder lists what you actually need to buy to build this. The component list is short because we're using standard parts that are available everywhere. Nothing proprietary, nothing hard to source.

## What You'll Need

**The main components:**

- **ESP32 DevKit V1** ($15) — The brain. 32-bit processor, runs the logic, reads sensors, controls the relay
- **Capacitive soil moisture sensor** ($5) — Goes in the plant soil. Outputs 0–4095 ADC based on wetness. We use capacitive (not resistive) because resistive probes corrode away if you leave them in soil
- **DHT22** ($5) — Reads temperature and humidity. Tells us if conditions are safe for watering
- **Float switch** ($2) — Sits in the water tank. Tells us if there's actually water to pump
- **1-channel relay module** ($3) — The pump switch. GPIO18 controls it. You need to verify if your specific module is active-high or active-low
- **5V DC mini pump** ($8) — The thing that actually moves water

**Accessories:**

- Breadboard and jumper wires (you probably have this)
- USB cable to program the ESP32
- 5V power adapter, minimum 2A (the pump draws a lot when it starts, so weak supplies cause the ESP32 to brownout)

See `bom.txt` for detailed sourcing from Robu.in, Amazon, and local electronics shops.

## Why Capacitive Not Resistive

Early prototypes used resistive soil probes. They corrode. After a few months of constant moisture, they stop working. Capacitive sensors use the soil dielectric instead of electrical resistance, so there's nothing to corrode away. They cost almost the same and last years instead of months.

## Relay Module Polarity

Relay modules come in two types: active-high (GPIO HIGH = pump on) and active-low (GPIO LOW = pump on). Check which one you got. You'll need to set `RELAY_ON` and `RELAY_OFF` in `firmware/include/config.h` to match.

## Power Architecture Note

The pump and relay need a separate 5V supply from the ESP32's power. Why? Pump inrush current can cause the ESP32 to reset mid-decision. We learned this the hard way—the system would start to water, the pump would draw too much current, the ESP32 would brown out, the relay would open, and it was chaos. Now we use independent rails with a shared ground. The ground connection is mandatory—if you float the supplies, the GPIO signals won't communicate correctly.

## What to Do With This List

1. Verify you have or can get all components listed above
2. Check the sourcing notes in `bom.txt` for specific part numbers and prices
3. Move to `wiring/` to see how to wire them together
