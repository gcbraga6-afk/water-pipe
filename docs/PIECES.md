# Water Pipe — Pieces

## 1. Piece model

A placed piece is defined by its type plus any applicable material or functional parameters.

Examples:

- copper straight pipe;
- PVC curve;
- copper valve;
- PVC T;
- pump 3.

A piece has at least a position, orientation, type, material/function, connection mask and gameplay state.

## 2. Basic pipes

### Straight

Two opposite connection ports. Rotates in 90° increments.

### Curve

Two adjacent connection ports. Rotates in 90° increments.

### T

Three connection ports. Passive and bidirectional.

### Cross

Four connection ports. Passive and bidirectional.

### Cap

Closes an endpoint and prevents water from escaping through that opening. It has no outgoing connection on the capped side.

## 3. Flow/control pieces

### Splitter 2-way

One incoming hydraulic flow is deliberately distributed between two outputs.

Unlike a passive T, the splitter is an explicit flow-control device.

### Splitter 3-way

One incoming hydraulic flow is deliberately distributed between three outputs.

### Mixer

Accepts multiple inputs and produces one mixed output. It is required to deliberately mix different-temperature waters.

### Valve

Has an open and closed state.

- Open: water passes.
- Closed: flow is held and pressure can increase.

The valve can be used to isolate sections and safely empty parts of a live installation.

## 4. Reservoir

Stores substantially more water than a normal pipe segment.

It can have connected input/output ports according to the phase configuration.

When full, additional incoming water causes overflow/loss.

## 5. Pumps

Four pump types exist:

| Type | Lift |
|---|---:|
| Pump 1 | +1 level |
| Pump 2 | +2 levels |
| Pump 3 | +3 levels |
| Pump 4 | +4 levels |

Pump strength is a gameplay abstraction for upward movement in side-view phases.

## 6. Materials

Initial materials:

| Material | Cost | Pressure resistance |
|---|---|---|
| PVC | Low | Low |
| Copper | High | High |

Exact numerical values are level-balancing parameters.

Material is selected after the player chooses the desired shape/function where the UI allows separate selection.

## 7. Orientation

Most one-cell pieces support four orientations.

The player rotates an already placed piece by tapping it.

A rotation must immediately update its connection mask and hydraulic connectivity.

## 8. Pipe states

A pipe may be:

- empty/dry;
- filling;
- containing water;
- under elevated pressure;
- critical;
- broken.

Broken is permanent until the piece is removed and replaced.

## 9. Placement rules

- Place only on empty buildable cells.
- Fixed infrastructure cannot be rotated or removed.
- Disconnected placement is allowed.
- Placement immediately consumes the phase's money/resource allocation.
- The piece does not need to be connected to be placed.

## 10. Removal rules

A piece containing water cannot be removed.

A dry piece can be removed, but its purchase cost is not refunded.

The game should clearly show when a piece is safe to remove.

## 11. Inventory modes

### NEXT PIPE

The phase provides a finite sequence of pieces.

- Current piece is mandatory.
- Two future pieces are previewed.
- HOLD stores one piece.
- After placement, the queue advances.

### FREE PICK

The player selects from a finite phase repository.

Resources are consumed when pieces are placed.

## 12. HOLD

HOLD behaves like a standard Tetris-style hold slot.

The player can store the current piece and use the held piece instead, subject to the phase's queue/resource rules.

The exact UI can remain compact because the hardware screen is only 800×480.

## 13. Future pieces

The following are intentionally outside the MVP catalog and may be introduced later:

- relief valve;
- one-way pipe;
- reducer;
- heater;
- cooler;
- leak pipe;
- steel material.

These should not be required by initial phases.
