# 📚 PLANT-CARE Documentation Index

**Quick Navigation for All Project Documents**

---

## 🚀 START HERE

### New to the Project?
**→ Read [`README.md`](README.md)** (5-10 min read)
- Project overview
- Architecture diagram
- Key features (safety mechanisms)
- Validation evidence

---

## 📋 Complete Documentation Map

### 1️⃣ Project Overview & Status
| Document | Purpose | Read Time |
|---|---|---|
| [`README.md`](README.md) | Project intro, features, architecture | 10 min |
| [`PROJECT_UPDATE_SUMMARY.md`](PROJECT_UPDATE_SUMMARY.md) | All changes made, status update | 15 min |
| [`IEEE_REPORT_DRAFT.md`](IEEE_REPORT_DRAFT.md) | Academic paper (version 2) | 20 min |

---

### 2️⃣ Hardware & Assembly
| Document | Purpose | Audience |
|---|---|---|
| [`hardware/BUILD_GUIDE.md`](hardware/BUILD_GUIDE.md) | **Step-by-step Stage 1 bench wiring** | Hardware builders |
| [`hardware/README.md`](hardware/README.md) | Build status, calibration data | Project managers |
| [`hardware/assembly_notes.txt`](hardware/assembly_notes.txt) | 4-day build log with lessons learned | Hardware developers |
| [`hardware/components_list.txt`](hardware/components_list.txt) | Bill of materials with costs | Procurement |

---

### 3️⃣ Firmware & Simulation
| Document | Purpose | Audience |
|---|---|---|
| [`firmware/main.ino`](firmware/main.ino) | Main irrigation control logic | Firmware developers |
| [`firmware/include/config.h`](firmware/include/config.h) | Configuration constants & GPIO pins | Tuning/Calibration |
| [`firmware/include/sensors.h`](firmware/include/sensors.h) | Sensor abstraction layer | Firmware developers |
| [`firmware/include/irrigation.h`](firmware/include/irrigation.h) | Pump control & safety logic | Firmware developers |
| [`firmware/include/telemetry.h`](firmware/include/telemetry.h) | Serial output formatting | Firmware developers |
| [`simulation/wokwi/README.md`](simulation/wokwi/README.md) | Wokwi simulator setup instructions | Simulation users |
| [`simulation/wokwi/sketch.ino`](simulation/wokwi/sketch.ino) | Simulation firmware (single file) | Simulation developers |
| [`simulation/wokwi/diagram.json`](simulation/wokwi/diagram.json) | Circuit schematic for Wokwi | Circuit designers |

---

### 4️⃣ Testing & Validation
| Document | Purpose | Audience |
|---|---|---|
| [`docs/simulation-validation-checklist.md`](docs/simulation-validation-checklist.md) | **100% validated** test scenarios | QA/Testers |
| [`SCREENSHOT_GUIDE.md`](SCREENSHOT_GUIDE.md) | How to capture Wokwi screenshots for paper | Paper authors |

---

### 5️⃣ Help & Troubleshooting
| Document | Purpose | Read When |
|---|---|---|
| [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md) | **30+ common issues & solutions** | Something doesn't work |
| [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md) | Tech specs for all IEEE figures & tables | Writing the paper |

---

## 🎯 Find What You Need

### "I want to..."

#### **Build the Hardware**
1. Read [`hardware/README.md`](hardware/README.md) for status
2. Follow [`hardware/BUILD_GUIDE.md`](hardware/BUILD_GUIDE.md) (detailed wiring steps)
3. Reference [`hardware/components_list.txt`](hardware/components_list.txt) for parts list
4. Check [`hardware/assembly_notes.txt`](hardware/assembly_notes.txt) for lessons learned

#### **Write the IEEE Paper**
1. Review [`IEEE_REPORT_DRAFT.md`](IEEE_REPORT_DRAFT.md) (current draft v2)
2. Use [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md) to create all 8 figures & 3 tables
3. Capture screenshots with [`SCREENSHOT_GUIDE.md`](SCREENSHOT_GUIDE.md)
4. Fill quantitative results using spec in [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md) Section "Table 3"

#### **Run Wokwi Simulation**
1. Setup with [`simulation/wokwi/README.md`](simulation/wokwi/README.md)
2. Validate with [`docs/simulation-validation-checklist.md`](docs/simulation-validation-checklist.md)
3. Capture evidence with [`SCREENSHOT_GUIDE.md`](SCREENSHOT_GUIDE.md)

#### **Debug a Problem**
1. Consult [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md) (indexed by issue type)
2. Check relevant firmware file: [`firmware/include/config.h`](firmware/include/config.h), `sensors.h`, `irrigation.h`
3. Review [`README.md`](README.md) Architecture section for expected behavior

#### **Understand the Architecture**
1. Read [`README.md`](README.md) sections: Project Overview, Architecture, Validation Evidence
2. View block diagram (future Figure 1 in [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md))
3. See flowchart (future Figure 2 in [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md))

#### **Modify Firmware**
1. Review [`firmware/include/config.h`](firmware/include/config.h) for tuning values
2. Edit relevant module: `sensors.h`, `irrigation.h`, `telemetry.h`
3. Rebuild using VS Code task: "Wokwi: Build Firmware"
4. Test in simulator

#### **Calibrate the Sensor**
1. See calibration values in [`hardware/README.md`](hardware/README.md)
2. Measure empirically using [`hardware/BUILD_GUIDE.md`](hardware/BUILD_GUIDE.md) Stage 1
3. Update [`firmware/include/config.h`](firmware/include/config.h):
   - `SENSOR_ADC_DRY` (value in air)
   - `SENSOR_ADC_WET` (value in water)
   - `SOIL_DRY_THRESHOLD_PERCENT` (trigger point)

---

## 📊 Document Types

### 📖 **Guides** (Step-by-step instructions)
- [`hardware/BUILD_GUIDE.md`](hardware/BUILD_GUIDE.md) - Wiring checklist & validation
- [`SCREENSHOT_GUIDE.md`](SCREENSHOT_GUIDE.md) - Evidence capture instructions

### 📋 **References** (Specifications & lookup tables)
- [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md) - All figures & tables for IEEE paper
- [`firmware/include/config.h`](firmware/include/config.h) - Configuration parameters
- [`hardware/components_list.txt`](hardware/components_list.txt) - Bill of materials

### 🆘 **Help** (Problem-solving)
- [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md) - 30+ issues organized by category

### 📝 **Logs & Status** (Historical & progress)
- [`hardware/assembly_notes.txt`](hardware/assembly_notes.txt) - 4-day build log
- [`hardware/README.md`](hardware/README.md) - Current build status
- [`PROJECT_UPDATE_SUMMARY.md`](PROJECT_UPDATE_SUMMARY.md) - Documentation updates

### 🎓 **Formal** (Academic & publishable)
- [`IEEE_REPORT_DRAFT.md`](IEEE_REPORT_DRAFT.md) - Paper draft (v2)
- [`README.md`](README.md) - Professional project summary

### ⚙️ **Code** (Implementation)
- [`firmware/main.ino`](firmware/main.ino) - Production firmware
- [`firmware/include/`](firmware/include/) - Modular headers
- [`simulation/wokwi/sketch.ino`](simulation/wokwi/sketch.ino) - Simulation code

---

## ✅ Quality Checklist

**Documentation Completeness**
- [x] Project overview complete
- [x] Hardware build guide with step-by-step wiring
- [x] Troubleshooting for 30+ common issues
- [x] Screenshot capture instructions for all Wokwi figures
- [x] Technical specifications for all IEEE paper figures & tables
- [x] Cross-references between all documents
- [x] Build status and calibration data current
- [x] Configuration values consistent across all files

---

## 🔗 Quick Links by File Extension

### `.ino` Files (Arduino Code)
- [`firmware/main.ino`](firmware/main.ino) - Main firmware
- [`simulation/wokwi/sketch.ino`](simulation/wokwi/sketch.ino) - Sim version

### `.h` Files (Modular Firmware)
- [`firmware/include/config.h`](firmware/include/config.h) - Config & pins
- [`firmware/include/sensors.h`](firmware/include/sensors.h) - Sensors
- [`firmware/include/irrigation.h`](firmware/include/irrigation.h) - Pump control
- [`firmware/include/telemetry.h`](firmware/include/telemetry.h) - Serial output

### `.json` Files (Configuration)
- [`simulation/wokwi/diagram.json`](simulation/wokwi/diagram.json) - Circuit diagram

### `.md` Files (Documentation)
- **Start here**: [`README.md`](README.md)
- **Build hardware**: [`hardware/BUILD_GUIDE.md`](hardware/BUILD_GUIDE.md)
- **Troubleshoot**: [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md)
- **Write paper**: [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md)
- **Capture screenshots**: [`SCREENSHOT_GUIDE.md`](SCREENSHOT_GUIDE.md)
- **Project status**: [`PROJECT_UPDATE_SUMMARY.md`](PROJECT_UPDATE_SUMMARY.md)
- **Navigation** (this file): [`DOCUMENTATION_INDEX.md`](DOCUMENTATION_INDEX.md)
- **Setup Wokwi**: [`simulation/wokwi/README.md`](simulation/wokwi/README.md)
- **Paper draft**: [`IEEE_REPORT_DRAFT.md`](IEEE_REPORT_DRAFT.md)
- **Validation**: [`docs/simulation-validation-checklist.md`](docs/simulation-validation-checklist.md)

### `.txt` Files (Text & Logs)
- [`hardware/assembly_notes.txt`](hardware/assembly_notes.txt) - Build log
- [`hardware/components_list.txt`](hardware/components_list.txt) - BOM
- [`simulation/wokwi/libraries.txt`](simulation/wokwi/libraries.txt) - Dependencies

---

## 📞 Getting Help

**1. Check the documentation first:**
   - Is it in [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md)?
   - Is there a guide: [`hardware/BUILD_GUIDE.md`](hardware/BUILD_GUIDE.md), [`SCREENSHOT_GUIDE.md`](SCREENSHOT_GUIDE.md)?
   - Check [`README.md`](README.md) Architecture for expected behavior

**2. Reference the specifications:**
   - [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md) for IEEE paper
   - [`firmware/include/config.h`](firmware/include/config.h) for parameters
   - [`hardware/components_list.txt`](hardware/components_list.txt) for parts

**3. Check logs & status:**
   - [`hardware/assembly_notes.txt`](hardware/assembly_notes.txt) for build history
   - [`hardware/README.md`](hardware/README.md) for current status
   - [`PROJECT_UPDATE_SUMMARY.md`](PROJECT_UPDATE_SUMMARY.md) for recent changes

---

## 🎯 Project Phases

### Phase 1: Digital Twin ✅ (Complete)
- Documentation: [`simulation/wokwi/README.md`](simulation/wokwi/README.md)
- Validation: [`docs/simulation-validation-checklist.md`](docs/simulation-validation-checklist.md) (100% ✓)

### Phase 2: Physical Hardware ⚠️ (In Progress)
- Guide: [`hardware/BUILD_GUIDE.md`](hardware/BUILD_GUIDE.md)
- Status: [`hardware/README.md`](hardware/README.md)
- Troubleshooting: [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md)

### Phase 3: Academic Paper ⚠️ (In Progress)
- Draft: [`IEEE_REPORT_DRAFT.md`](IEEE_REPORT_DRAFT.md)
- Specifications: [`FIGURE_SPECIFICATIONS.md`](FIGURE_SPECIFICATIONS.md)
- Evidence capture: [`SCREENSHOT_GUIDE.md`](SCREENSHOT_GUIDE.md)

### Phase 4: Documentation ✅ (Complete)
- All guides written
- All specifications provided
- Cross-references established

---

**Last Updated**: 2026-04-09  
**Documentation Version**: 2.0 (Comprehensive)  
**Total Documents**: 20+  
**Total Documentation**: 100+ KB

---

**👉 Next Step**: Go to [`README.md`](README.md) or select from the list above based on what you need to do.
