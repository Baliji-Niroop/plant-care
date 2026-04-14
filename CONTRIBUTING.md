# Contributing to ESP32 Smart Plant Care

Thank you for wanting to improve this project. This document explains how to do that in a way that keeps the system reliable and maintainable.

## What We Actually Care About

This project prioritizes:

- **Correctness over features** — We'd rather have a system that reliably does one thing than a system that half-does ten things. Safety and accuracy matter more than feature count.

- **Clarity over brevity** — Code and documentation need to be understandable to someone who didn't write it. We explain not just *what* we're doing, but *why*.

- **Consistency over cleverness** — We follow established patterns and naming conventions so the codebase feels cohesive. New contributors shouldn't have to figure out three different styles.

- **Validation over assumption** — Everything is tested. If you claim something works, you show evidence it works. No "seems fine" or "should be okay."

Contributions that improve the project without introducing hidden assumptions or breaking established patterns are welcome.

---

## Before You Start

**Take 15 minutes and read these:**

1. [`README.md`](./README.md) — What this system actually does and why
2. [`docs/architecture/architecture.md`](./docs/architecture/architecture.md) — How the decision logic works
3. [`docs/deployment/workflow.md`](./docs/deployment/workflow.md) — End-to-end workflow and decision procedures

**Then, based on what you're changing, read:**

- **Changing how sensors work?** → `docs/calibration/calibration.md` explains calibration and thresholds
- **Modifying workflows?** → `docs/deployment/deployment.md` explains what must stay synchronized
- **Adding features?** → `docs/validation/simulation-validation-checklist.md` shows how features are validated
- **Updating hardware wiring?** → `hardware/README.md` shows pin mappings and electrical design
- **Working on the simulation?** → `simulation/wokwi/README.md` explains simulation-specific differences

This reading isn't optional. Changes that contradict documented design or introduce undocumented assumptions will be requested to change.

---

## Firmware Changes

If you're modifying files in `firmware/` or `firmware/include/`, verify all of this before submitting:

**Pin and configuration validation:**
- All GPIO references match actual ESP32 pins (verify against the datasheet if you're unsure)
- Pin numbers in config.h match pin numbers in code
- No two signals use the same GPIO
- Relay and tank switch polarity settings are documented and intentional

**Safety logic stays intact:**
- The 6-stage decision hierarchy is preserved (or changes are clearly justified)
- Tank interlock and watchdog protections are not bypassed
- Error handling catches edge cases (sensor disconnected, ADC out of range, etc.)

**Code quality:**
- Comments explain *why* code exists, not what it does (code that's clear doesn't need "read GPIO5")
- Variable and function names match existing conventions (don't use `readSoil()` if everything else uses `acquireSoil()`)
- Telemetry output remains human-readable and diagnostic
- All magic numbers live in config.h with clear labels

**Validation:**
- Code compiles without warnings using Arduino CLI: `arduino-cli compile --fqbn esp32:esp32:esp32 firmware/firmware.ino`
- Simulation still builds and runs (`Wokwi: Build Firmware` task)
- Any behavior changes are reflected in validation scenarios

**Documentation stays synchronized:**
- If code structure changes, update `firmware/README.md`
- If logic flow changes, update `docs/architecture/architecture.md`
- If pin mapping changes, update `hardware/README.md`

---

## Simulation Changes

If you're modifying files in `simulation/wokwi/`:

**Keep simulation clearly marked as simulation-only:**
- Sketches include a prominent warning about not flashing to hardware
- All intentional differences from production are documented in `simulation/wokwi/README.md`

**Synchronize with production carefully:**
- Document every intentional difference (timing, polarity, mapping, etc.)
- Explain *why* each difference exists
- Keep decision logic structure aligned with production where possible

**Build and execution:**
- Changes work with Wokwi CLI and local execution
- `wokwi.toml` configuration remains valid
- Scripts referenced in `README.md` still execute correctly

**Validation:**
- Test scenarios in `docs/validation/simulation-validation-checklist.md` still pass
- New behavior has corresponding validation steps
- Telemetry output format stays consistent

---

## Documentation Changes

Documentation updates are where the project's clarity lives or dies. When updating docs:

**Keep information synchronized across files:**
- GPIO pin numbers are the same everywhere they're mentioned
- Calibration thresholds are the same in every document
- Timing parameters match across references
- Relay and switch polarity settings are consistent

Use a text search to verify—if you change "GPIO5" to "GPIO6" somewhere, find all other places it's mentioned.

**Make narrative sense:**
- Problem-to-solution should flow logically
- Design decisions are explained with reasoning, not just stated
- Workflows progress from high-level overview to detailed steps
- Links between documents are accurate and useful

**Think about the reader:**
- Content serves both beginners (learning) and experienced developers (reviewing)
- Jargon is explained the first time it appears
- Complex diagrams include text explanations
- Examples are realistic and actually runnable

**Completeness:**
- Table of contents updated if structure changed
- Code snippets are syntactically correct
- File paths are relative to repo root or clearly specified
- Deprecated content is removed, not abandoned

---

## Commit Quality

Commits should be clear and purposeful. A good commit message tells someone what changed and why.

**Good commit message:**
```
Add watchdog timer for pump runtime protection

- Enforces 30-second maximum runtime for safety
- Emergency stop triggered if runtime exceeded
- Telemetry reports watchdog activation events
- Verified in simulation scenarios 4 and 5

Refs: docs/architecture/architecture.md#safety-mechanisms
```

**Bad commit messages:**
- "fixed stuff" — what stuff?
- "updates" — which updates?
- Mixing 5 unrelated changes in one commit
- Including debug print statements or commented code
- Reformatting code and fixing logic in the same commit

Group related changes. Keep commits focused on one thing. If your change affects multiple modules, explain why they're related.

---

## Pull Requests

When you open a PR, include:

**What you changed and why:**
- Reference the issue this addresses (if applicable)
- Explain what problem you're solving
- Note any new dependencies or requirements
- Call out breaking changes clearly

**Proof it works:**
- Include screenshots or telemetry excerpts showing validation
- Reference specific test scenarios you ran
- Note any edge cases you tested
- Link to validation procedures in the docs

**Scope:**
- Keep PRs focused on one concern
- Split large refactors into smaller PRs
- Talk to maintainers before tackling significant changes

---

## Code of Conduct

This is how we treat each other:

**Expected:**
- Respectful, constructive communication
- Assume good faith in questions and suggestions
- Feedback focuses on ideas, not individuals
- Welcome questions from people of all skill levels

**Not acceptable:**
- Harassment or discriminatory language
- Dismissive comments about someone's experience or ability
- Gatekeeping or elitism
- Impatient or hostile tone

Issues or PRs that violate this will be closed.

---

## Questions?

If you're stuck:

1. Read this document thoroughly
2. Check existing issues and PRs to see if someone asked the same thing
3. Open an issue with a specific scenario describing what you're trying to do

We're here to help you contribute well.
