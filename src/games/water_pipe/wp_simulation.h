#pragma once
// Hydraulic simulation: connectivity, progressive filling and open-end
// loss (docs/WATER_SYSTEM.md, docs/TECHNICAL_DESIGN.md sections 6-8).
//
// MVP scope only (docs/TECHNICAL_DESIGN.md section 24): no pressure,
// temperature, valves or gravity yet. Connectivity is re-evaluated every
// tick from the live board, so placing/rotating a piece while water is
// flowing reconnects or disconnects the network automatically.
//
// Pure logic — no rendering or hardware dependency.

#include "wp_board.h"
#include "wp_levels.h"

namespace wp {

struct SimState {
    bool waterActive = false;    // true once the phase's water has started
    bool hasStarted = false;     // true once the source delay has elapsed
    uint32_t delayRemainingMs = 0;
    uint32_t accumulatorMs = 0;

    bool reached[BOARD_CELLS] = {};
    bool leaking[BOARD_CELLS] = {};

    int totalLoss = 0;
    int delivered = 0;
    bool targetReached = false;
    bool victory = false;

    // True only for the tick that just ran, if it grew the connected
    // network by at least one cell. Used to detect a truly stalled
    // network (docs/GAMEPLAY.md section 20: "objective becomes
    // mathematically impossible").
    bool lastTickProgressed = false;
};

// Resets simulation state and arms the level's source delay. Call once
// after Board::loadLevel().
void resetSimulation(SimState &state, const Level &level);

// Advances the simulation by dtMs of wall-clock time, running as many
// fixed SIM_TIMESTEP_MS steps as needed. No-op once state.victory is set.
void tickSimulation(SimState &state, Board &board, const Level &level, uint32_t dtMs);

// Defeat is only meaningful once the network has genuinely stalled: no
// growth on the last tick, the target hasn't been reached yet, and the
// player has no pieces left to place (queue + hold both empty).
inline bool isDefeated(const SimState &state, bool inventoryEmpty) {
    return state.hasStarted && !state.victory && !state.targetReached &&
           !state.lastTickProgressed && inventoryEmpty;
}

}  // namespace wp
