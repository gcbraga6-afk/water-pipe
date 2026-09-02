# Water Pipe

Water Pipe is a construction-and-action hydraulic puzzle game built around visible water flow, pressure, temperature, and reactive problem solving.

## Core loop

**Think → Build → Activate → Observe → React → Correct**

The player builds hydraulic systems and manages the live installation as water flows through it. The game is inspired by real-world hydraulic behavior, simplified where necessary for readability and fun.

## Documentation

- `docs/GAMEPLAY.md` — complete gameplay specification
- `docs/WATER_SYSTEM.md` — water, flow, volume, pressure, temperature, and gravity rules
- `docs/PIECES.md` — pipe pieces, equipment, materials, and inventory modes
- `docs/LEVEL_DESIGN.md` — phase structure, objectives, progression, and level philosophy
- `docs/TECHNICAL_DESIGN.md` — architecture, data layout, MVP scope and implementation order
- `docs/HARDWARE.md` — the Frida/CrowPanel host API (canvas, touch, audio, memory limits) a game module must integrate against

## Target platform

Initial target: **CrowPanel Advance 7-inch** hardware project (ESP32-S3, Frida firmware — see `docs/HARDWARE.md`).

## Code

`src/games/water_pipe/` implements the MVP scope from `docs/TECHNICAL_DESIGN.md` section 24: a 10×7 board, straight/curve/T/cross/cap pieces, one source and target, progressive filling, open-end loss, placement/rotation/removal, a NEXT PIPE queue with HOLD, restart/exit controls, campaign menu, phase selection, persistent resume, stars and high scores — as a single introductory phase ("Phase 1 - Connect").

The Water Pipe main menu provides **Continue / Phases / Scores / Exit**. Leaving an active phase saves the exact board, simulation and inventory state so Continue can restore it later. Completed phases record a score and best star count.

The hydraulic simulation and board logic have no rendering or hardware dependency. `src/games/water_pipe/water_pipe.h` exposes `begin()`/`loop()`, the same contract the host firmware uses for every game (see `docs/HARDWARE.md` section 6).

`src/core/` contains compatibility stubs for the firmware's `GfxCompat`/`TouchDriver`/`audio` host API, so the game builds and runs standalone on desktop. **Replace `src/core/` with the real Frida firmware files when integrating** — everything under `src/games/` is written directly against the documented host API and needs no changes.

### Build and test (native, desktop)

```
cmake -S . -B build
cmake --build build
./build/wp_tests   # pure-logic checks: pieces, board rules, simulation, inventory, progress
./build/wp_demo    # builds the reference solution via simulated taps and writes wp_demo.ppm
```
