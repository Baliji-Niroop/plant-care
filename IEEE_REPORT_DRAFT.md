# IEEE Paper Working Draft

## Metadata

- Title: Autonomous Plant Irrigation System Using ESP32 and Soil Moisture Sensing
- Author: Niroop Baliji
- Affiliation: Department of Automobile Engineering, VNR VJIET
- Keywords: ESP32, IoT, smart irrigation, embedded control, safety interlock

## Abstract

Small-scale plant care often depends on manual irrigation, which introduces inconsistent watering frequency and avoidable water wastage. This work presents an ESP32-based autonomous irrigation controller that combines soil moisture sensing, ambient monitoring, and tank-level safety checks to make local watering decisions without cloud dependency. The implementation uses a dual-track workflow: modular firmware for physical deployment and a Wokwi digital twin for repeatable simulation and fast validation. The control logic activates pumping only when soil moisture is below threshold and tank-water availability is confirmed. To improve operational safety, three mechanisms are included: tank interlock, cooldown delay, and watchdog timeout. Validation in the digital twin demonstrates expected behavior for baseline wet-soil hold, dry-soil activation, and tank-empty safety override, with telemetry traces used for verification. The proposed approach emphasizes reliability, reproducibility, and low-cost implementation suitable for student projects and early prototype deployment. Future work includes hardware-stage performance quantification, long-duration field logging, and remote telemetry integration.

## 1. Introduction

Irrigation automation is a practical use case for embedded systems in low-cost agriculture and home gardening environments. Manual watering is typically rule-of-thumb based and can cause either under-watering or over-watering, especially when caretakers are unavailable. A microcontroller-driven irrigation node can reduce this variability by using periodic sensor measurements and deterministic decision logic.

This project targets a compact, educationally realistic architecture based on the ESP32 platform. The proposed controller reads soil moisture as a primary watering indicator, while DHT22 temperature and humidity data provide environmental context. A tank-level input enforces dry-run prevention by blocking pump activation when water is not available.

The main contribution of this work is a dual-track engineering workflow: production-oriented modular firmware and a local digital twin that mirrors control behavior. This enables fast test cycles, easier debugging, and documented validation evidence before full hardware assembly.

## 2. Literature Survey

Recent IoT irrigation literature commonly reports three recurring design dimensions: sensing strategy, decision intelligence, and communication stack. Most low-cost systems use moisture-threshold logic for actuation due to simplicity, while advanced systems include weather prediction or machine learning at increased complexity. Safety handling is often under-emphasized in student-level prototypes, where pump dry-run, relay chatter, and indefinite ON states can degrade reliability.

Compared to cloud-first architectures, local-decision embedded designs are easier to reproduce in constrained lab settings and remain functional during network outages. This project intentionally prioritizes local control correctness and safety sequencing, then stages cloud telemetry as future scope. The identified gap addressed here is an end-to-end workflow that links modular firmware design with simulation-backed validation artifacts suitable for academic reporting.

## 3. System Architecture

The system is organized into three layers:

1. Embedded firmware layer for deployment-ready logic.
2. Digital twin layer in Wokwi for deterministic validation.
3. Documentation layer for traceable evidence and report integration.

The hardware interface includes an ESP32 DevKit V1, capacitive soil sensor, DHT22, relay-driven pump output, and tank-level switch. Firmware modules separate configuration, sensing, irrigation decisions, and telemetry output. The digital twin mirrors this behavior in a single-file simulation sketch with equivalent state transitions for rapid testing.

Planned figure: Figure 1 - System block diagram (ESP32 -> Sensors -> Decision Engine -> Relay/Pump).

## 4. Implementation

### 4.1 Hardware Design

The primary pin mapping uses ESP32 ADC and digital IO as follows: soil sensor on GPIO34 (analog), DHT22 data on GPIO4, tank switch on GPIO5, and relay input on GPIO18. Relay polarity differs across platforms; production configuration is active-high while the Wokwi relay component is active-low. This difference is documented in the simulation code to prevent interpretation errors.

### 4.2 Firmware Design

The firmware follows a periodic sensing and decision loop. Key configurable values include moisture threshold, pump ON duration, cooldown interval, and watchdog timeout. In the modular configuration, threshold and timing defaults are set to practical conservative values for hardware safety.

State handling is represented as monitoring, watering, cooldown, and error modes. The controller enters watering only when dry-soil criteria are met and tank availability is true. Telemetry prints timestamped data including sensor values, pump state, cooldown state, and last decision outcome for auditability.

### 4.3 Safety Mechanisms

Three safety mechanisms are implemented:

1. Tank interlock: blocks pump if tank input indicates low or empty state.
2. Cooldown delay: enforces a pause between watering cycles to avoid oscillation.
3. Watchdog timeout: hard-stops pump if run duration exceeds expected limit.

These controls are intended to reduce dry-run risk, relay stress, and unintended continuous operation.

## 5. Simulation and Testing

Validation is performed using a local Wokwi digital twin and a checklist-driven procedure. Baseline validation confirms that wet-soil conditions hold the pump OFF. Scenario-based tests then force dry-soil values to trigger pump activation and toggle tank-empty state to verify safety override.

Required validation evidence includes:

1. System overview screenshot.
2. Pump-active condition screenshot.
3. Safety-interlock condition screenshot.
4. Optional cooldown telemetry sequence screenshot.

The simulation workflow compiles firmware artifacts and records serial decision traces. This provides reproducible logic validation before physical-stage integration.

## 6. Results and Discussion

Initial digital twin results confirm the expected state transitions for core scenarios. The pump remains OFF under wet-soil baseline, activates under dry-soil conditions, and is blocked when tank-water is unavailable. Cooldown and watchdog logic provide additional behavioral constraints that improve robustness compared to basic threshold-only control.

Current limitations include lack of long-duration hardware data, no quantified water-volume savings, and placeholder evidence images pending replacement with final captures. Nevertheless, the present results demonstrate correctness of decision flow and safety-first behavior under controlled simulation conditions.

### 6.1 Quantitative Results Template (Fill During Final Run)

Use this subsection to convert observations into measurable outcomes.

- Number of simulation runs completed: [N]
- Pump activation success rate under dry-soil condition: [x/N] = [..%]
- Tank-empty interlock success rate: [x/N] = [..%]
- Watchdog intervention count (expected 0 under nominal settings): [N]
- Mean pump ON duration per watering event: [..] ms
- Mean cooldown duration observed: [..] ms

Recommended table format for final manuscript:

| Scenario | Expected Decision | Observed Decision | Pass/Fail | Evidence |
|---|---|---|---|---|
| Wet soil baseline | hold_soil_ok, pump OFF | [fill] | [fill] | Fig. [x] |
| Dry soil | watering_start, pump ON | [fill] | [fill] | Fig. [x] |
| Tank low interlock | hold_tank_empty, pump OFF | [fill] | [fill] | Fig. [x] |
| Cooldown behavior | cooldown sequence observed | [fill] | [fill] | Fig. [x] |

### 6.2 Discussion Points for Final Version

Use the following prompts while finalizing the discussion:

1. Compare threshold response speed between simulation and hardware (once hardware data is available).
2. Explain any false triggers and how debounce plus cooldown reduced them.
3. Comment on relay polarity differences between Wokwi and production wiring.
4. State whether the chosen threshold prioritizes plant safety or water conservation.

## 7. Conclusion and Future Work

This work demonstrates a practical ESP32 irrigation controller with explicit safety controls and a simulation-backed validation method. The architecture is intentionally modular and reproducible, making it suitable for coursework, demonstrations, and incremental prototype development.

Future work will include:

1. Physical prototype assembly and calibration-based threshold tuning.
2. Multi-day deployment tests with measured moisture and pump-cycle trends.
3. Optional cloud telemetry dashboard for remote monitoring and analytics.
4. Comparative analysis against manual watering schedules.

## 8. Figures and Tables Checklist

Figures to add:

- Figure 1: System block diagram.
- Figure 2: Irrigation control flowchart.
- Figure 3: Wokwi circuit snapshot.
- Figure 4: Pump-active validation evidence.
- Figure 5: Tank-interlock validation evidence.
- Figure 6: Cooldown behavior evidence.
- Figure 7: Stage 1 hardware photo.
- Figure 8: Stage 2 hardware photo.

Tables to add:

- Table 1: Bill of materials and estimated cost.
- Table 2: Firmware configuration parameters and defaults.
- Table 3: Validation scenario outcomes.

## 9. References Plan (IEEE Style)

### 9.1 Source Mix Target

For a balanced literature section, target:

1. 3 papers on IoT or smart irrigation control.
2. 1 paper on soil-moisture sensor calibration or reliability.
3. 1 paper on embedded safety or fail-safe actuation.
4. 1 paper on digital twin or simulation-driven embedded validation.

### 9.2 Search Queries to Use

Use these exact query strings in IEEE Xplore, ScienceDirect, Springer, or Google Scholar:

1. "ESP32 smart irrigation soil moisture threshold"
2. "IoT irrigation system relay pump control safety"
3. "soil moisture sensor calibration capacitive agriculture"
4. "digital twin embedded systems validation"
5. "low cost automatic irrigation controller"

### 9.3 IEEE Citation Templates

Fill these templates with verified metadata from selected papers.

- Journal article:
	[n] A. Author and B. Author, "Paper title," Journal Name, vol. x, no. y, pp. xx-yy, Month Year, doi: xx.xxxx/xxxx.

- Conference paper:
	[n] A. Author, "Paper title," in Proc. Conference Name, City, Country, Year, pp. xx-yy, doi: xx.xxxx/xxxx.

- Standard or technical report:
	[n] Organization, "Document title," Standard/Report ID, Year. [Online]. Available: URL

### 9.4 Working Reference Slots

Replace each slot below with a real paper before submission:

- [1] Smart irrigation architecture (core system paper)
- [2] Soil-moisture threshold control paper
- [3] ESP32 or low-cost IoT irrigation implementation paper
- [4] Pump safety or actuator fail-safe paper
- [5] Digital twin or simulation validation paper
- [6] Comparative irrigation performance paper

## Status

Draft upgraded to version 2 with a fill-ready quantitative results template and a structured references workflow. Next pass should focus on inserting real citations, replacing evidence placeholders with final figures, and adding measured hardware data.
