# Project Assets

This folder stores diagrams, screenshots, and images for the project. Purely visual stuff—nothing code-related, just things that help explain how the system looks and behaves.

## What's Here

**`diagrams/`** — System architecture and wiring diagrams. Visual representations of how components connect and how data flows through the system. These get embedded in the documentation.

**`screenshots/`** — Real captured images showing the system in action. Wokwi simulator screenshots showing the relay state and pump behavior. Serial telemetry output showing what the controller is thinking. Hardware assembly photos if you want to document the build process.

**`demo-images/`** — Portfolio and presentation stuff. Photos of the final working system, close-ups of components, the plant actually being watered. Useful if you're showing this off or using it in documentation.

## How to Add Images

When you add diagrams or photos:
1. Put them in the right folder (`diagrams/`, `screenshots/`, or `demo-images/`)
2. Use kebab-case naming (like `system-architecture.svg` not `SystemArchitecture.png`)
3. When you reference them in markdown docs, use relative paths: `![alt text](../assets/diagrams/your-image.svg)`

The `.gitkeep` files just hold the folder structure—git won't preserve empty folders otherwise.
