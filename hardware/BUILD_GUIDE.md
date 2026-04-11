# Hardware Build Guide (Stage 1: Bench Prototype)

This guide helps you assemble and verify the system on a breadboard before enclosure/PCB work.

## 1) Before you start

Checklist:
- [ ] All parts available (`hardware/components_list.txt`)
- [ ] ESP32 flashed with `firmware/firmware.ino`
- [ ] USB cable for serial monitor
- [ ] Breadboard + jumper wires
- [ ] Separate 5V supply for relay/pump

## 2) Wiring order (recommended)

### Step A: Common power and ground

Connect:
- External 5V+ to breadboard positive rail
- External GND to breadboard ground rail
- ESP32 GND to same ground rail

Keep a **common ground** between ESP32 and external supply.

### Step B: Soil moisture sensor

| Sensor pin | ESP32 | Notes |
|---|---|---|
| VCC | 3.3V | safer for ADC logic |
| GND | GND | common ground |
| AO | GPIO 34 | analog input |
| DO | NC | not used |

Quick check:
- dry air gives high raw ADC (around 3950)
- wet water gives lower raw ADC (around 1650)

### Step C: DHT22

| DHT pin | ESP32 | Notes |
|---|---|---|
| VCC | 3.3V | do not use 5V |
| GND | GND | common ground |
| DATA | GPIO 4 | digital data line |
| NC | NC | unused |

Quick check:
- readings appear in serial
- first few startup failures can be normal

### Step D: Float switch (tank level)

| Switch side | ESP32 | Notes |
|---|---|---|
| Terminal A | GPIO 5 | input pin |
| Terminal B | GND | with pull-up logic |

Default firmware behavior:
- `INPUT_PULLUP`
- tank available when GPIO5 reads `LOW`

So default wiring is GPIO5 ↔ switch ↔ GND.

### Step E: Relay module

| Relay pin | Connection | Notes |
|---|---|---|
| VCC | External 5V | relay supply |
| GND | Common GND | shared reference |
| IN | GPIO 18 | control signal |
| COM | External 5V+ | switched source |
| NO | Pump positive | switched output |

Important:
- Relay polarity depends on module type.
- Set `RELAY_ON` and `RELAY_OFF` in `firmware/include/config.h`.

### Step F: Pump

| Pump wire | Connection |
|---|---|
| Pump + | Relay NO |
| Pump - | External supply GND |

Safety check:
- pump should be OFF at boot
- pump should run only when relay command is ON

## 3) Wiring reference diagram

- Mermaid source: `hardware/wiring-diagram.mmd`

Use this as the final cross-check after manual wiring.

## 4) Stage-1 validation flow

### Serial monitor

Open serial monitor at `115200`.
Expected startup text includes:
- `ESP32 Smart Plant Irrigation`
- `System ready. Monitoring started.`

### Functional tests

1. Dry-soil trigger:
   - make soil dry
   - wait for debounce readings
   - expect pump start

2. Wet-soil hold:
   - make soil wet again
   - expect hold / pump off

3. Tank-empty override:
   - simulate empty tank (float low)
   - even with dry soil, pump must stay off

4. Cooldown check:
   - after one watering cycle, trigger dry again quickly
   - pump should not restart until cooldown ends

## 5) Troubleshooting table

| Problem | Likely reason | Action |
|---|---|---|
| No serial logs | wrong COM/baud/driver | verify port and 115200 |
| Soil stuck at 0 or 4095 | bad AO wire | recheck GPIO34 and wiring |
| DHT failures every cycle | power/noise/wire issue | recheck 3.3V, GND, data |
| Tank state never changes | switch wiring issue | verify GPIO5 and switch orientation |
| Relay always ON/OFF wrong | polarity mismatch | update `RELAY_ON/RELAY_OFF` |
| Pump does not run | relay or pump power path wrong | verify COM/NO and 5V supply |
| ESP32 resets on pump start | supply sag | use stronger separate 5V supply |

## 6) Evidence to capture

After validation:
1. top-view wiring photo
2. side photo during pump operation
3. serial screenshots for key scenarios
4. update `hardware/README.md` with stage completion note

## 7) Next practical steps

- [ ] re-check calibration values in `config.h`
- [ ] run longer soak test
- [ ] prepare enclosure or proto-board migration
