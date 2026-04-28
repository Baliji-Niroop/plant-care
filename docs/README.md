# Documentation Index

All the docs live here, organized by what they help you do. Most people start with the root README, but depending on what you're working on, you might jump straight to architecture if you want to understand the decision logic, or deployment if you want to get something running.

## What's Where

**[Architecture](./architecture/architecture.md)** — How the decision logic actually works. The system checks six constraints in order before running the pump. This explains why, the sequence, what each check does, and how they interact. Start here if you want to understand the engineering.

**[Calibration](./calibration/calibration.md)** — Your specific soil sensor probably won't read the same values as our sensor. This explains how to measure your soil's dry and wet points, then map those ADC values to percentages. You only need this if you're deploying to production.

**[Deployment](./deployment/)** — How to get the system running.
  - **[Deployment Guide](./deployment/deployment.md)** — Step-by-step: build the simulation, test it, then move to hardware if you want

**[Validation](./validation/)** — How we test the system. Includes scenarios like "dry soil should water" and "tank empty should block watering." If you modify the logic, you should verify these still pass.

**[Troubleshooting](./troubleshooting/)** — Real problems we hit during prototype. DHT22 gave NaN values until we added warm-up time. Relay oscillated until we added debounce. Pump failed until we used separate power rails. Every safety feature is documented with the problem it solved.

## Quick Paths

- **I want to learn:** Start with root `README.md`, then read `architecture/architecture.md`
- **I want to try the simulation:** Follow `deployment/deployment.md` then check `validation/simulation-validation-checklist.md`
- **I want to build the hardware:** Start with `hardware/wiring/build-guide.md`, then come back to docs/troubleshooting if something doesn't work
- **I'm tuning for my own soil:** Read `calibration/calibration.md` before deploying to your plants
