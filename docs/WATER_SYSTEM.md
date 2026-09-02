# Water Pipe — Water System

This document defines the gameplay simulation of water, volume, flow, pressure, temperature and gravity. It is intentionally simplified so it can run efficiently on the target ESP32-S3 hardware.

## 1. Simulation philosophy

The game does not require physically accurate fluid dynamics. It uses a deterministic network simulation that produces convincing visual behavior.

The hydraulic network is a graph:

- cells/pieces are nodes;
- compatible openings are edges;
- water volume is stored on segments/nodes;
- pressure is an abstract state used to resolve restrictions and rupture;
- temperature is carried with water volume.

## 2. Water volume

Every normal pipe segment has a fixed gameplay capacity.

Water fills progressively. When the segment reaches capacity, additional available water continues into connected downstream segments.

Water must never disappear merely because a segment is full. If there is nowhere valid for incoming water to go, the excess becomes pressure/overflow according to the local situation.

Recommended initial implementation representation:

- volume: fixed-point integer or small integer unit;
- capacity: integer units;
- temperature: fixed-point value;
- pressure: fixed-point value.

Avoid floating-point-heavy simulation where integer/fixed-point arithmetic is sufficient.

## 3. Flow connectivity

A connection is valid only when both pieces expose a compatible opening toward each other.

For each simulation step:

1. Identify active connected components beginning at sources or already active stored water.
2. Determine available outgoing connections for each active cell.
3. Exclude closed valves, blocked construction restrictions and unavailable routes.
4. Distribute flow among remaining routes.
5. Move volume according to capacity and the phase flow rate.
6. Record overflow, pressure changes and losses.

Disconnected pieces remain inactive.

## 4. Flow splitting

When a node has multiple equally available outgoing routes, flow is divided evenly.

- 2 routes → 50% each.
- 3 routes → approximately 33% each.

If a route is blocked or unavailable, it receives no normal flow and pressure accumulates toward the restriction.

The system may have multiple simultaneous flow directions. A junction does not require one globally defined input direction.

## 5. Meeting flows

When flows from different directions reach the same hydraulic volume, they become part of the same connected water system.

For temperature, however, different-temperature water should retain separate volume identities until it passes through a Mixer. This allows hot and cold streams to meet without automatically producing a mixed temperature.

The implementation can therefore represent a segment as one or more water packets when necessary, or use a compact source/temperature contribution representation.

## 6. Pressure

Pressure is an abstract scalar.

Pressure increases when incoming flow cannot be moved forward at the expected rate. Typical causes:

- closed valve;
- full segment with continued incoming flow;
- blocked branch;
- insufficient pumping/lift in side-view;
- other phase-defined restrictions.

Pressure decreases when a restriction is removed and water can move again.

Pressure should change gradually rather than jump instantly, giving the player a reaction window.

## 7. Pressure states

Suggested normalized states:

- 0–49% of material limit: normal.
- 50–74%: medium warning.
- 75–94%: high warning.
- 95–100%: critical warning.
- >100%: rupture.

Exact thresholds can be tuned during testing.

Visual feedback should be stronger than numeric feedback.

## 8. Rupture

When pressure exceeds the pipe's material limit:

1. Mark the pipe as broken.
2. Create an open leak at the rupture point.
3. Continue moving water toward the opening.
4. Count escaping water as loss.
5. Prevent repair.
6. Allow eventual removal only after the pipe is dry.

If pressure is reduced while still below the limit, the pipe returns to its normal state.

## 9. Materials

Each material defines at least:

- purchase cost;
- pressure limit;
- visual appearance.

Initial materials:

| Material | Relative cost | Pressure resistance |
|---|---:|---:|
| PVC | Low | Low |
| Copper | High | High |

Exact numerical values are balancing parameters.

## 10. Temperature

Temperature is carried with the water.

Initially, heat loss is ignored. A water packet therefore maintains its temperature while moving through normal pipes.

A Mixer deliberately combines water packets.

For two packets:

`T_mix = (V1*T1 + V2*T2) / (V1 + V2)`

Example:

- 10 L at 90°C
- 10 L at 10°C
- result: 20 L at 50°C

The same rule extends naturally to more than two inputs.

## 11. Mixer behavior

A Mixer waits for its input volumes and combines them into a common output volume.

The output temperature is the volume-weighted average.

A short visual mixing effect can be triggered when the mixed output is produced.

## 12. Gravity

Top-down phases use no vertical gravity calculation.

Side-view phases introduce a simple height constraint.

Water can move downward naturally. Upward movement requires sufficient pressure or pump assistance.

The game does not need continuous hydrostatic equations. Instead, compare the required height increase against available lift/pressure.

## 13. Pumps

Pump strength is measured in grid levels:

- Pump 1 → +1 level.
- Pump 2 → +2 levels.
- Pump 3 → +3 levels.
- Pump 4 → +4 levels.

Multiple pumps can be chained.

A pump provides a deterministic upward-lift allowance rather than a full mechanical pump simulation.

## 14. Sources

A source supplies water at a phase-defined rate.

A finite source also has a remaining volume.

A continuous source can be treated as having a sufficiently large/infinite gameplay budget until the objective is complete.

A source may also have a programmed start delay.

## 15. Loss

Water leaving the intended hydraulic network through:

- open pipe ends;
- ruptures;
- reservoir overflow;
- phase-defined leaks

is counted as lost water.

Loss is accumulated for objectives and star scoring.

## 16. Reservoirs

Reservoirs use the same volume concept but have a substantially larger capacity.

When a reservoir reaches capacity:

- further incoming water is overflow/loss;
- outgoing connected flow may continue normally if an outlet exists.

## 17. Simulation timing

The water simulation should update at a fixed, stable interval independent of rendering details.

Rendering can interpolate visual filling between simulation states if useful, but gameplay state should remain deterministic.

The target hardware should avoid allocating temporary objects every frame. Prefer fixed-size arrays and reusable buffers.

## 18. Determinism

Given the same:

- phase;
- piece layout;
- inventory;
- source configuration;
- player actions;

simulation results should be reproducible.

This is important for debugging, balancing and reliable star scoring.
