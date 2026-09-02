# Water Pipe — Gameplay Specification

## 1. Concept

Water Pipe is a construction-and-action hydraulic puzzle game. The player builds a hydraulic installation, activates or manages the water supply, watches the system react, and corrects problems while water is flowing.

Core loop:

**Think → Build → Activate → Observe → React → Correct**

The game aims for the feel of real hydraulic installations while using simplified, readable game physics.

## 2. Board

- Standard board: 10 × 7 grid on the 800 × 480 CrowPanel display.
- Most pieces occupy one cell.
- Special pieces may occupy multiple cells when required by a phase.
- The whole map is visible during play.
- Players may build on any empty cell unless an obstacle or construction restriction prevents it.
- Some infrastructure can be pre-built and fixed.
- The source connection is pre-built.

## 3. Construction and touch controls

- Tap an empty cell to place the currently selected piece.
- Tap an existing pipe briefly to rotate it 90 degrees clockwise.
- Press and hold an existing pipe for approximately 450 ms to remove it.
- The hold gesture is only a removal gesture; it does not also rotate the pipe.
- Pieces appear immediately; there is no placement animation.
- Pieces may be placed while disconnected from the active water system.
- There is no undo.
- A mistake must be corrected by removing/rebuilding the affected installation.

The separate on-screen remove-mode toggle is intentionally not used. The same board touch area has a simple two-action vocabulary: **tap = rotate, hold = remove**.

## 4. Removal

- A pipe can only be removed when it contains no water.
- Removing a dry piece refunds nothing: the money spent on it is lost.
- A live installation can be reworked using valves and waiting for sections to empty.
- A broken pipe cannot be repaired. It must eventually be removed and replaced once dry.

## 5. Water

Water is a real gameplay resource represented as volume.

- Water is visible inside pipes.
- Pipes fill progressively.
- Normal pipe segments have limited capacity.
- Reservoirs have much larger capacity.
- Some phases have finite water supplies.
- Other phases have effectively continuous supply until the objective is completed.
- Water escaping through an open endpoint is lost.
- A phase may tolerate a limited amount of loss or may require containment with zero loss.

## 6. Connections

Water flows only through valid reciprocal pipe connections. A connection exists when both adjacent pieces expose compatible openings toward each other.

Disconnected pipes remain inactive until connected to an active hydraulic system.

Straight, curve, T and cross pieces are passive and bidirectional.

## 7. Flow

- Water travels through the connected hydraulic network.
- Water fills each available segment progressively up to its capacity before continuing.
- Flow can exist in different directions in different parts of the network.
- When currents meet, they mix into the same system rather than one current automatically winning.
- At a junction with multiple free routes, water divides between the available routes.
- With two equally free routes, each receives 50%.
- With three equally free routes, each receives approximately 33%.
- A blocked or restricted route does not receive normal flow when another route is free; pressure accumulates at the restriction instead.

## 8. Pressure

Pressure is always part of the simulation but is only strongly visualized when elevated.

Pressure rises when flow is obstructed, including:

- closed valves;
- full pipes receiving additional water;
- attempting to climb in side-view without sufficient lift;
- other restrictions introduced by later phases.

## 9. Pressure limits and rupture

- Pressure is normalized for gameplay and can exceed the safe operating range before rupture.
- Proposed visual thresholds: 0–49 normal, 50–74 medium, 75–94 high, 95–100 critical, above 100 rupture.
- Normal pressure is blue.
- Medium pressure is stronger/pulsing blue.
- High pressure is purple/intense/pulsing.
- Critical pressure is red with faster pulsing and shaking.
- PVC has a lower pressure limit than copper.
- The player receives a clear critical warning before rupture.
- Opening a valve lowers pressure gradually.
- If pressure falls below the critical range before rupture, the system returns to a safe visual state.
- A rupture is permanent and creates a new opening/loss point. If the objective becomes impossible, the phase ends immediately.

## 10. Temperature

Temperature is a property of the water and is introduced in later phases.

- Sources may provide different temperatures.
- Water carries its temperature without meaningful heat loss initially.
- Different-temperature streams remain separate until they enter a Mixer.
- A Mixer combines streams using volume-weighted average temperature.
- Example: 10 L at 90°C plus 10 L at 10°C produces 20 L at 50°C.
- Example: 10 L at 90°C plus 2 L at 10°C remains close to 90°C.
- A short mixing animation communicates the transition to a uniform temperature.
- Temperature can be represented visually by water color and used by phase objectives.
- Material temperature effects are deferred; pressure determines pipe strength initially.

## 11. Gravity and side-view

Top-down phases ignore height.

Side-view phases introduce gravity and vertical lift.

- Water seeks lower levels.
- Water cannot automatically climb above available pressure/lift.
- Pumps provide additional lift.
- Pump strengths are +1, +2, +3 and +4 grid levels.
- Multiple pumps can be chained.
- Height and pressure interact.
- The visual layout must make elevation understandable without requiring text instructions.

## 12. Pieces

Core pieces:

- Straight
- Curve
- T
- Cross
- Cap
- Splitter 2-way
- Splitter 3-way
- Mixer
- Valve
- Pump 1–4
- Reservoir
- PVC
- Copper

Future optional pieces include relief valves, one-way pipes, reducers, heaters, coolers, leak pipes and steel.

## 13. Inventory

- Standard mode is NEXT PIPE: current piece plus two upcoming pieces.
- One HOLD slot is available.
- HOLD behaves like a Tetris-style hold and is subject to its placement cooldown rule.
- Inventory is finite per phase.
- Later modes may allow FREE PICK.

## 14. Materials and economy

- PVC is cheaper and has lower pressure resistance.
- Copper is more expensive and has higher pressure resistance.
- Money is deducted when a piece is placed.
- Removing a dry piece does not refund its cost.
- Initial economy is per-level budget rather than a persistent campaign economy.

## 15. Reservoirs

Reservoirs have much larger capacity than normal pipe segments.

- A reservoir can store and release water through connected outlets.
- A full reservoir receiving additional water can overflow and lose volume.

## 16. Sources

- A phase can have one or two sources.
- Sources can be located at an edge or in a cell.
- The source connection is pre-built.
- Some sources start immediately; others may have a delay such as 10 seconds.
- Finite sources stop when their available water is consumed.
- Continuous sources keep supplying until the objective is completed.
- The player builds while water is flowing.

## 17. Objectives

There is no universal win condition.

Examples:

- connect source A to target B;
- contain all water;
- deliver a target volume of warm water;
- keep water loss below a limit;
- keep pressure below a limit;
- fill a reservoir;
- deliver water to a target elevation;
- combinations of the above.

Objectives should be communicated primarily through the physical installation and visual targets.

## 18. Victory

- Victory occurs immediately when the objective is satisfied.
- A short victory animation may play while the completed hydraulic system remains visible.
- Completed phases remain replayable.

## 19. Defeat

Defeat occurs immediately when the objective becomes mathematically impossible.

Examples include:

- water loss exceeds an unavoidable limit;
- required resources are exhausted;
- a rupture makes the objective impossible;
- a phase-specific failure condition is reached.

Some phases may use countdowns; others use time only for star scoring.

## 20. Stars

Each phase awards up to three stars.

- One star: complete the objective.
- Two stars: strong water efficiency.
- Three stars: strong overall efficiency.
- Exact thresholds are balance data per phase.
- Stars do not block campaign progression.

## 21. Campaign structure

- Worlds/chapters contain multiple phases.
- The world map is used to select phases.
- Completed phases are replayable.
- Difficulty increases naturally through new mechanics and combinations rather than explicit Easy/Normal/Hard modes.

Suggested progression:

- World 1 — Basic Water: grid, straight/curve, T/cross, source/target, flow, loss and capacity.
- World 2 — Control and Pressure: valves, finite water, pressure, PVC limits and rupture.
- World 3 — Capacity and Distribution: reservoirs, splitters, multiple routes and volume objectives.
- World 4 — Height: side-view, gravity and pumps.
- World 5 — Temperature: hot/cold sources, separate streams and Mixer.
- Later worlds combine the systems.
