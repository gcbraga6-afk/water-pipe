# Water Pipe — Technical Design

## 1. Target

Initial target: CrowPanel Advance 7-inch based on ESP32-S3.

The game must be designed around the constraints of the device rather than assuming a desktop-class engine.

Known target characteristics:

- ESP32-S3-WROOM-1
- dual core, up to 240 MHz
- 8 MB PSRAM
- 800 × 480 RGB565 display
- GT911 touch, single point
- lightweight drawing API
- game framebuffer already available in the host game environment
- text rendering is not assumed to be available initially

## 2. Architecture

The game is divided into five layers:

1. **Game State** — phase, board, inventory, money, objectives and progression.
2. **Hydraulic Simulation** — connectivity, water, pressure, temperature and gravity.
3. **Input** — converts touch into game actions.
4. **Renderer** — draws the board and current state.
5. **Audio/UI Integration** — communicates with the existing host application without owning the global device lifecycle.

The hydraulic simulation must not depend on rendering code.

## 3. Proposed source layout

```text
src/
└── games/
    └── water_pipe/
        ├── water_pipe.h
        ├── water_pipe.cpp
        ├── wp_types.h
        ├── wp_board.h
        ├── wp_board.cpp
        ├── wp_simulation.h
        ├── wp_simulation.cpp
        ├── wp_pieces.h
        ├── wp_pieces.cpp
        ├── wp_levels.h
        ├── wp_levels.cpp
        ├── wp_render.h
        ├── wp_render.cpp
        └── wp_input.h
```

The exact filenames can change during implementation, but responsibilities should remain separated.

## 4. Fixed-size data

Because the board is only 10 × 7, the core simulation should use fixed-size arrays.

Maximum normal board cells:

`70`

Avoid dynamic allocation in the main game loop.

Recommended approach:

- fixed arrays for cells;
- fixed arrays for water packets;
- fixed arrays for BFS/DFS queues;
- reusable temporary buffers;
- no per-frame `String` construction;
- no per-frame heap allocation.

## 5. Core board representation

Each cell contains a compact piece state.

Conceptual fields:

```text
pieceType
rotation
material
flags
volume
pressure
waterTemperature
```

A piece's connection mask is derived from its type and rotation.

Use four direction bits:

```text
UP    = 1
RIGHT = 2
DOWN  = 4
LEFT  = 8
```

Examples:

- straight horizontal = RIGHT + LEFT
- straight vertical = UP + DOWN
- curve up-right = UP + RIGHT
- cross = all four bits

## 6. Simulation step

The simulation runs at a fixed timestep.

Each step:

1. Read source supply.
2. Discover active hydraulic connectivity.
3. Determine valid outgoing routes.
4. Distribute available flow.
5. Transfer water between cells.
6. Apply capacity limits.
7. Update pressure.
8. Apply gravity/lift rules for side-view.
9. Update temperature packets.
10. Process overflow/leaks/ruptures.
11. Evaluate objectives.
12. Mark changed cells for rendering.

Rendering is separate and may run more frequently than simulation if necessary.

## 7. Connectivity algorithm

For each source:

- start a queue at the source cell;
- traverse only reciprocal connections;
- stop at blocked/closed boundaries;
- mark reachable cells as active.

Because the board is only 60 cells, a simple BFS is sufficient and preferable to a complex graph library.

## 8. Flow algorithm

Flow is represented as volume transfer per simulation step.

For each active cell:

- identify valid exits;
- determine how many are available;
- split outgoing flow evenly when routes are equivalent;
- constrain transfer by destination capacity and local restrictions;
- unresolved incoming flow increases pressure.

The algorithm must support water arriving from multiple directions.

## 9. Water representation

Initial implementation may use a compact water-packet model.

A packet contains:

- volume;
- temperature;
- optional source identity.

A cell may need more than one packet when hot and cold water occupy the same segment without a Mixer.

Because the board is small, a fixed upper bound on packets per cell should be chosen during implementation. If packet counts become excessive, compatible packets may be compacted without changing gameplay results.

## 10. Pressure model

Pressure is an integer/fixed-point scalar.

Recommended normalized range:

`0..1000`

Material limits can then be represented with the same scale.

Suggested visual thresholds:

- <500: normal
- 500–749: medium
- 750–949: high
- 950–1000: critical
- >1000: rupture

These values are initial implementation defaults and may be tuned.

## 11. Material defaults

Initial balancing defaults:

| Property | PVC | Copper |
|---|---:|---:|
| Cost | 10 | 25 |
| Pressure limit | 700 | 1000 |

These numbers are deliberately simple starting points. They are expected to be tuned after the first playable prototype.

## 12. Water defaults

Initial balancing defaults:

| Property | Value |
|---|---:|
| Normal pipe capacity | 10 volume units |
| Reservoir capacity | 100 volume units |
| Base source flow | 2 volume units / simulation step |
| Simulation timestep | 100 ms |

The visual water speed can be tuned independently from the logical volume scale.

## 13. Temperature

Use integer degrees Celsius for gameplay.

Mixer calculation:

`mixedTemperature = (sum(volume × temperature)) / totalVolume`

Round to the nearest integer.

Do not simulate heat loss in the MVP.

## 14. Side-view

Each cell receives a logical height coordinate.

Top-down phases use a flat height model.

Side-view phases use height differences when determining whether water can advance upward.

Pump lift is represented as a discrete level bonus:

- P1 = 1
- P2 = 2
- P3 = 3
- P4 = 4

## 15. Input

Touch mapping:

- tap empty cell → place selected piece;
- tap occupied rotatable piece → rotate;
- dedicated UI control → change inventory/selection;
- dedicated control → HOLD;
- menu control → restart/exit.

Input handling should be edge-triggered so one physical touch cannot accidentally rotate/place multiple times.

## 16. Rendering

The game uses incremental rendering.

Do not clear and redraw the entire 800 × 480 display every frame unless profiling proves it is safe.

Track dirty cells and redraw only:

- changed board cells;
- animated water regions;
- pressure warning states;
- affected UI elements.

The board should occupy most of the display while leaving a compact HUD/inventory region.

## 17. Visual language

The renderer should communicate gameplay without relying on text.

Important states need distinct visual signatures:

- dry pipe;
- active water;
- water filling;
- pressure buildup;
- critical pressure;
- leak/rupture;
- hot water;
- cold water;
- mixed water;
- valve open/closed;
- pump strength.

The visual style should be realistic enough to resemble real installations but stylized enough to remain immediately readable.

## 18. Level data

Levels should be data-driven.

A phase definition contains:

- view type;
- board layout;
- fixed pieces;
- obstacles;
- sources;
- targets;
- starting money;
- inventory mode;
- piece sequence/repository;
- source rate and volume;
- source delay;
- objective parameters;
- time limit;
- star thresholds.

The simulation engine must not contain level-specific rules hard-coded into piece behavior.

## 19. Game state machine

Suggested states:

```text
MENU
  ↓
PHASE_LOADING
  ↓
BUILDING
  ↓
RUNNING
  ↓
VICTORY / DEFEAT
  ↓
RESULT
```

BUILDING and RUNNING are allowed to overlap logically because the player can build while water is flowing. Therefore the implementation should treat `waterActive` as a simulation flag rather than making RUNNING an exclusive input mode.

## 20. Objective evaluation

Objectives should be represented by small independent evaluators.

Examples:

- delivery amount;
- reservoir fill;
- maximum loss;
- temperature range;
- pressure maximum;
- height reached;
- time limit.

Each evaluator reports:

- current progress;
- satisfied;
- failed/impossible.

The phase combines these evaluators according to its objective definition.

## 21. Victory/defeat detection

Victory is checked after each simulation update and after relevant player actions.

Defeat is triggered when a required condition becomes mathematically impossible, not merely because a temporary intermediate state is bad.

Examples:

- loss exceeds a hard limit;
- required water has been permanently lost;
- required resources cannot build any valid remaining solution;
- rupture makes the target unreachable.

## 22. Audio

The game must use the host application's asynchronous audio mechanism.

The game loop should enqueue sound effects rather than directly writing audio data.

Suggested events:

- piece placed;
- piece rotated;
- valve opened/closed;
- water started;
- pressure warning;
- rupture;
- mixer activation;
- victory;
- defeat.

## 23. Performance targets

Target:

- stable 60 FPS rendering when practical;
- fixed simulation timestep;
- no repeated heap allocation during gameplay;
- no expensive full-screen redraw every frame;
- simulation comfortably below the frame budget on the ESP32-S3.

If necessary, rendering can be reduced while keeping the hydraulic simulation stable.

## 24. MVP scope

The first playable build should include only:

- 10 × 7 top-down board;
- straight;
- curve;
- T;
- cross;
- cap;
- one source;
- one target;
- basic water volume;
- progressive filling;
- open-end loss;
- rotation;
- placement;
- removal of dry pieces;
- restart;
- victory/defeat;
- basic HUD;
- one introductory phase.

Do **not** implement temperature, pumps, side-view, complex pressure, world map or all inventory mechanics in the first prototype.

They should be added incrementally after the basic loop is proven on hardware.

## 25. Implementation order

1. Host game integration.
2. 10 × 7 board rendering.
3. Touch placement and rotation.
4. Basic piece connection masks.
5. Source and target.
6. Connectivity BFS.
7. Water volume/filling.
8. Open-end loss.
9. Victory/defeat.
10. Removal rules.
11. First playable phase.
12. Hardware testing and performance tuning.
13. Pressure/valve.
14. Reservoir/splitters.
15. Side-view/pumps.
16. Temperature/mixer.
17. Campaign/map/inventory expansion.

The first milestone is not "complete Water Pipe". It is **one fun, stable hydraulic puzzle running on the actual CrowPanel hardware**.
