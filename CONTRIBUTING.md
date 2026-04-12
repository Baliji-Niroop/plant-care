# Contributing

## 1. Purpose

This repository prioritizes correctness, readability, and consistency across firmware, simulation, and documentation.

Contributions should improve quality without changing technical truth unless the change is intentionally implemented and validated.

## 2. Contribution principles

- Keep behavior aligned with existing architecture.
- Do not introduce undocumented assumptions.
- Keep production and simulation differences explicit.
- Preserve beginner-friendly documentation quality.
- Keep naming and terminology consistent with existing files.

## 3. Before you submit changes

1. Read `README.md` for project overview.
2. Read `docs/architecture/architecture.md` for system structure.
3. If changing calibration-sensitive behavior, review `docs/calibration/calibration.md`.
4. If changing workflows, update `docs/deployment/deployment.md` and relevant folder docs.

## 4. Firmware changes checklist

If you modify files in `firmware/` or `firmware/include/`:

- verify pin references and constants in `firmware/include/config.h`
- keep safety logic intact unless intentionally changed
- keep comments accurate and concise
- ensure decision reasons and telemetry remain understandable

## 5. Simulation changes checklist

If you modify files in `simulation/wokwi/`:

- preserve clear labeling that simulation firmware is not production firmware
- keep intentional simulation differences documented
- ensure scripts and readme remain synchronized

## 6. Documentation changes checklist

- keep cross-file values synchronized (pins, thresholds, timings, polarity)
- avoid duplication across documents where possible
- keep structure clear: problem -> design -> workflow -> validation
- write for both beginners and technical reviewers

## 7. Validation expectations

At minimum, verify:

- build still succeeds in simulation workflow
- checklist scenarios remain valid in `docs/validation/simulation-validation-checklist.md`
- no contradictions were introduced in docs

## 8. Commit quality expectations

- use clear, focused commit messages
- group related changes logically
- avoid mixing unrelated refactors in one commit

## 9. Code of conduct

Use respectful, constructive communication in issues and pull requests.
