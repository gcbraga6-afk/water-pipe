# Water Pipe — Gameplay Specification

## 1. Concept

Water Pipe is a construction-and-action hydraulic puzzle game. The player builds a hydraulic installation, activates or manages the water supply, watches the system react, and corrects problems while water is flowing.

Core loop:

**Think → Build → Activate → Observe → React → Correct**

The game aims for the feel of real hydraulic installations while using simplified, readable game physics.

## 2. Board

- Standard board: 10 × 6 grid.
- Most pieces occupy one cell.
- Special pieces may occupy multiple cells when required by a phase.
- The whole map is visible during play.
- Players may build on any empty cell unless an obstacle or construction restriction prevents it.
- Some infrastructure can be pre-built and fixed.
- The source connection is pre-built.

## 3. Construction

- Tap an empty cell to place the currently selected piece.
- Tap an existing pipe to rotate it 90 degrees.
- Pieces appear immediately; there is no placement animation.
- Pieces may be placed while disconnected from the active water system.
- There is no undo.
- A mistake must be corrected by removing/rebuilding the affected installation.

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

Opening a restriction allows pressure to fall progressively.

Pressure is intentionally simplified rather than physically simulated.

Visual states:

- Normal: stable blue water.
- Medium: stronger/pulsing blue.
- High: intense/purple pulsing.
- Critical: red, faster pulsing and pipe shaking.

Critical pressure provides a short reaction window before rupture.

## 9. Rupture

Each material has a pressure limit.

If pressure exceeds the material limit, the pipe ruptures.

- PVC has a lower pressure limit.
- Copper has a higher pressure limit.
- A rupture creates a permanent opening.
- Water can continue escaping through the broken pipe.
- The pipe cannot be repaired.
- The player must eventually remove it after it becomes dry and replace it.
- If the rupture makes the objective impossible, the phase ends in defeat immediately.
- If pressure is reduced before rupture, the pipe returns to its normal state; there is no intermediate damaged-but-functional state.

## 10. Temperature

Temperature belongs to the water volume.

- Temperature is carried through the hydraulic network.
- Heat loss is ignored initially.
- Different-temperature waters do not automatically become mixed merely because their flows meet.
- A Mixer is required for deliberate mixing.
- Mixing uses a volume-weighted average temperature.
- Example: 10 L at 90°C plus 10 L at 10°C produces 20 L at approximately 50°C.
- Example: 10 L at 90°C plus 2 L at 10°C remains much closer to 90°C.
- A short visual mixing animation can be used before the mixed volume becomes uniform.

Temperature objectives may require a target range such as 35–45°C.

## 11. Gravity and side-view

Top-down phases ignore physical height for gameplay purposes.

Side-view phases introduce height:

- Water naturally seeks lower levels.
- Water does not automatically climb above the available pressure/lift.
- Pumps provide reliable upward movement.
- Height and pressure interact.

Top-down and side-view are separate phase types.

## 12. Pumps

Four pump strengths exist:

- Pump 1: +1 grid level.
- Pump 2: +2 grid levels.
- Pump 3: +3 grid levels.
- Pump 4: +4 grid levels.

Multiple pumps may be used in series.

Stronger pumps cost more and are visually distinct.

## 13. Valves

Valves are placeable pieces.

- Open valve: water passes.
- Closed valve: water is held back.
- Continued input against a closed valve increases pressure progressively.
- Opening the valve allows the stored pressure to fall progressively.
- Valves can therefore be used to control a live installation and create opportunities to safely empty sections before rebuilding.

## 14. Reservoirs

Reservoirs store substantially more water than normal pipe segments.

- Water can enter and accumulate.
- Water can leave through connected outlets.
- A full reservoir receiving more water causes overflow/loss.

## 15. Sources

A phase can have one or two water sources.

- Source connection starts pre-built.
- A source may be represented at the edge of the board or as a source cell, depending on the phase.
- Some phases start immediately.
- Others start after a programmed delay, such as 10 seconds.
- A visual/audio warning can precede delayed activation.
- Once active, a finite source continues until its available water is consumed; a continuous source continues until the phase objective ends.

## 16. Piece queue and inventory

Two main inventory modes exist.

### NEXT PIPE

The player must use the current queue piece.

- Current piece plus two future pieces are visible.
- After placement, the queue advances immediately.
- HOLD works like a Tetris-style hold slot.
- Inventory is finite per phase rather than infinite.

### FREE PICK

The player chooses any available piece from the phase's repository.

- The repository is limited.
- Each piece consumes resources when placed.
- Running out of resources can make the objective impossible and therefore causes defeat.

Recommended selection hierarchy: choose shape first, then material/function when applicable.

## 17. Materials and economy

PVC and copper are the initial materials.

### PVC

- Cheap.
- Lower pressure resistance.

### Copper

- More expensive.
- Higher pressure resistance.

Money is a per-phase resource, not a persistent campaign economy in the initial design.

Money is deducted when a piece is placed.

A more expensive but safer construction is a valid solution.

## 18. Objectives

There is no universal objective. Each phase defines one or more goals, such as:

- connect source A to destination B;
- contain all water;
- produce a target volume of warm water;
- lose no more than a specified volume;
- remain below a pressure limit;
- fill a reservoir;
- deliver water to a target height;
- combinations such as delivering 100 L at 40–45°C with no more than 10 L loss.

When delivery is the objective, the destination should be represented as a physical/visual object.

## 19. Victory

Victory occurs immediately when the phase objective is satisfied.

The hydraulic system remains visible during a short victory animation.

## 20. Defeat

A phase is lost when the objective becomes mathematically impossible, including cases such as:

- water loss exceeds the phase limit;
- required resources are exhausted;
- a rupture or other failure makes the objective impossible;
- a phase-specific fail condition is reached.

## 21. Time

Time has two possible roles:

- Some phases have a real countdown and reaching zero causes defeat.
- Other phases have no time-based failure; time is only relevant to star performance.

There is no speed-up button.

Active gameplay is not paused during normal play.

## 22. Stars

Every phase awards up to three stars.

The baseline is:

- 1 star: complete the objective.
- 2 stars: complete it with strong water efficiency.
- 3 stars: complete it with strong overall efficiency, using phase-appropriate measures such as water loss, money/resources and/or time.

Exact thresholds are level-specific and are balancing data rather than core simulation rules.

Stars encourage replay and optimization but do not block campaign progression.

## 23. Progression

Campaign structure:

**World/Chapter → Phases**

Phase selection uses a world map.

- New mechanics are introduced progressively.
- Completed phases can be replayed.
- There is no explicit Easy/Normal/Hard selector.
- Difficulty increases naturally through objectives, restrictions, combinations and environments.
- The phase shows the problem/environment, not its solution.
- Some phases may contain pre-built infrastructure.

## 24. Phase types

The game supports distinct phase formats, including:

- top-down construction;
- side-view construction with gravity and height;
- one-source systems;
- two-source systems;
- phases with pre-existing infrastructure;
- phases with immediate water;
- phases with delayed activation;
- phases with finite water;
- phases with continuous supply;
- phases combining multiple simultaneous objectives.

The player can build while water is flowing.

## 25. Design philosophy

Water Pipe should feel like a puzzle with action rather than a static logic puzzle.

The intended emotional rhythm is:

**Plan → Build → Start → Watch → Panic a little → React → Recover → Succeed**

The game should use visual teaching rather than text-heavy tutorials. Real-world hydraulic installations are the inspiration, while exaggerated water movement, pressure warnings, leaks and ruptures provide game readability and fun.

Multiple valid solutions should exist whenever practical. The game evaluates the player's result, not whether they discovered one predetermined construction.

## 26. Initial piece catalog

Core MVP pieces:

- Straight pipe
- Curve pipe
- T pipe
- Cross pipe
- Cap
- Valve
- Splitter 2-way
- Splitter 3-way
- Mixer
- Reservoir
- Pump 1
- Pump 2
- Pump 3
- Pump 4
- PVC material
- Copper material

Potential future equipment such as relief valves, one-way pipes, reducers, heaters, coolers, leak pipes and steel are not required for the MVP.

## 27. Restart

Restart is available through the game/menu interface and completely resets the current phase to its initial state.
