# Water Pipe — Level Design

## 1. Campaign structure

The campaign is organized as:

**World/Chapter → Phases**

Phase selection uses a world map.

Completed phases remain replayable.

There is no explicit Easy/Normal/Hard selector. Difficulty increases naturally through the campaign.

## 2. Level philosophy

Every phase presents a concrete hydraulic problem or environment. The phase shows the situation and constraints, not a revealed solution.

Multiple valid solutions are preferred.

The player should learn mechanics primarily through visual interaction and level construction rather than text-heavy tutorials.

## 3. Introduction of mechanics

Mechanics should be introduced one at a time or in small combinations.

Suggested progression:

### World 1 — Basic Water

Introduce:

- grid construction;
- straight and curve pipes;
- T and cross;
- source and destination;
- simple water flow;
- open-end water loss;
- caps.

### World 2 — Control and Pressure

Introduce:

- valves;
- finite water;
- pressure buildup;
- PVC pressure limits;
- rupture and recovery/rebuild.

### World 3 — Capacity and Distribution

Introduce:

- reservoirs;
- splitters;
- multiple routes;
- water volume objectives;
- more complex networks.

### World 4 — Height

Introduce:

- side-view phases;
- gravity;
- pumps 1–4;
- height objectives;
- pressure/lift interaction.

### World 5 — Temperature

Introduce:

- hot/cold sources;
- separate-temperature streams;
- mixer;
- volume-weighted mixing;
- temperature-range objectives.

Later worlds can combine all previously introduced mechanics and add new equipment or environments.

## 4. Phase formats

A phase can vary along several dimensions:

- top-down or side-view;
- one or two sources;
- immediate or delayed water start;
- finite or continuous water supply;
- empty board or pre-built infrastructure;
- one objective or multiple simultaneous objectives;
- NEXT PIPE or FREE PICK inventory.

These dimensions should be combined carefully so each new mechanic remains understandable.

## 5. Objectives

Examples:

- connect source to target;
- deliver a specified amount of water;
- fill a reservoir;
- contain all water;
- keep loss below a limit;
- remain below a pressure limit;
- deliver water to a specific height;
- produce a target temperature range;
- satisfy combinations of volume, temperature, loss, pressure and time.

Objectives should be represented visually wherever practical.

For example, a delivery target is a physical object on the board rather than only a sentence in a menu.

## 6. Timing

Some phases have an actual countdown. Reaching zero causes defeat.

Other phases have no countdown; time is only used for performance/star scoring.

Some phases may delay source activation by a programmed period. A visual/audio warning should communicate the impending start.

Once water starts, the player can continue building. There is no pause during normal active gameplay and no speed-up button.

## 7. Pre-built infrastructure

Pre-built elements can create more interesting scenarios:

- fixed pipes;
- fixed valves;
- fixed reservoirs;
- existing damaged/broken sections when appropriate;
- fixed environmental obstacles.

Fixed infrastructure cannot be altered unless the specific phase explicitly allows it.

## 8. Obstacles

Obstacles may be:

- physical blockers that occupy board space;
- construction restrictions that prevent placement;
- hydraulic restrictions that affect flow;
- environmental height/route constraints in side-view.

The distinction should be visually obvious.

## 9. Inventory design

### NEXT PIPE phases

Use a curated finite sequence to create tactical decisions. Show the current piece plus two future pieces and provide one HOLD slot.

### FREE PICK phases

Provide a finite repository of pieces and materials. The player has freedom of construction but must manage limited resources.

The phase designer controls the repository, costs and starting money.

## 10. Stars

Every phase has three stars.

Baseline:

- ⭐ Complete the objective.
- ⭐⭐ Complete with strong water efficiency.
- ⭐⭐⭐ Complete with strong overall efficiency.

The exact thresholds should be configured per phase. Appropriate metrics include:

- water lost;
- money spent;
- pieces used;
- completion time;
- pressure events.

Stars encourage replay and optimization but do not block progression.

## 11. Difficulty progression

Difficulty should come from combinations rather than simply making water move faster.

Examples:

- more constrained build area;
- more complex routes;
- multiple sources;
- delayed activation;
- limited inventory;
- pressure-sensitive materials;
- height changes;
- temperature requirements;
- simultaneous objectives;
- reduced tolerance for water loss.

The player should usually understand the problem before understanding every possible solution.

## 12. Example early phases

### Phase 1 — Connect

One source, one target, simple top-down board. Straight and curve pieces. Objective: deliver water to the target.

### Phase 2 — Don't Spill

Introduce caps and open-end loss. Objective: deliver water without exceeding a small loss allowance.

### Phase 3 — Junction

Introduce T and cross pieces. Objective: route water around a blocked area.

### Phase 4 — First Valve

Introduce a valve and live construction. The player learns that a valve can isolate part of the system while water continues flowing elsewhere.

### Phase 5 — Pressure

Use a closed valve and PVC to create a visible pressure problem. The player must react before rupture.

### Phase 6 — Storage

Introduce the reservoir and a fill-volume objective.

### Phase 7 — Split

Introduce a 2-way splitter and require controlled distribution.

### Phase 8 — Height

Switch to side-view and introduce Pump 1.

### Phase 9 — More Height

Introduce stronger pumps and chained pumping.

### Phase 10 — Hot + Cold

Introduce two sources with different temperatures and a Mixer. Objective: produce a target temperature range.

These are examples, not a locked final campaign list.

## 13. Level data

A phase should ultimately be representable as data rather than hard-coded game logic.

Conceptually a phase contains:

- grid dimensions;
- camera/view type;
- obstacles;
- fixed infrastructure;
- sources;
- destinations/targets;
- starting money;
- available pieces;
- inventory mode;
- source timing;
- source volume/rate;
- objective definitions;
- loss limits;
- time limits when applicable;
- star thresholds;
- visual environment/theme.

The implementation should make it possible to add a new phase without changing the hydraulic engine.

## 14. Environment progression

The game should gradually move from abstract training installations toward recognizable real-world environments.

Possible environments include:

- utility rooms;
- homes;
- gardens;
- industrial facilities;
- farms;
- pools;
- construction sites;
- water treatment infrastructure.

Environment changes should affect the visual context and available problems, not require a separate game system unless needed.
