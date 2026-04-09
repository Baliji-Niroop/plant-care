# IEEE Conference Paper Draft Outline

## Metadata

- Title: Autonomous Plant Irrigation System Using ESP32 and Soil Moisture Sensing
- Author: Niroop Baliji
- Affiliation: Department of Automobile Engineering, VNR VJIET
- Keywords: ESP32, IoT, plant irrigation, embedded systems, automation, safety interlock

## Abstract

Draft target: 150 to 200 words.

To include:
- Problem statement and motivation
- Proposed architecture (firmware plus digital twin)
- Safety features and validation method
- Key outcomes and future scope

## 1. Introduction

- Need for low-cost autonomous irrigation
- Challenges in overwatering and dry-run protection
- Objectives and contribution summary

## 2. Literature Survey

- Review 5 to 6 relevant IoT irrigation papers
- Compare sensing, control logic, and connectivity choices
- Identify gap addressed by this implementation

## 3. System Architecture

- Hardware block diagram
- Firmware module map
- Digital twin workflow in Wokwi

## 4. Implementation

### 4.1 Hardware Design

- ESP32 board and pin mapping
- Soil moisture, DHT22, relay, and tank switch integration

### 4.2 Firmware Design

- Sensor acquisition flow
- Irrigation state decisions
- Telemetry and diagnostics

### 4.3 Safety Mechanisms

- Tank interlock
- Cooldown anti-chatter
- Watchdog timeout cutoff

## 5. Simulation and Testing

- Validation scenarios from checklist
- Screenshot-backed evidence
- Repeatability notes and limitations

## 6. Results and Discussion

- Decision correctness across scenarios
- Safety behavior under edge conditions
- Comparison to baseline manual irrigation

## 7. Conclusion and Future Work

- Summary of achieved objectives
- Hardware pilot deployment plan
- Future cloud telemetry and analytics

## 8. References

Use IEEE citation format consistently.

## Planned Figures

- Figure 1: System block diagram
- Figure 2: Control flowchart
- Figure 3: Wokwi circuit schematic snapshot
- Figure 4: Pump-active validation screenshot
- Figure 5: Safety interlock validation screenshot
- Figure 6: Cooldown behavior validation screenshot
- Figure 7: Stage 1 hardware photo
- Figure 8: Stage 2 hardware photo

## Planned Tables

- Table 1: Hardware bill of materials
- Table 2: Firmware configuration parameters
- Table 3: Validation scenario outcomes

## Status

Outline complete. Awaiting finalized screenshots, hardware photos, and measured results for full manuscript drafting.
