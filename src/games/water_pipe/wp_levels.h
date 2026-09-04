#pragma once
// Data-driven level definitions (docs/LEVEL_DESIGN.md section 13,
// docs/TECHNICAL_DESIGN.md section 18). The simulation/board engine must
// not hard-code level-specific rules — everything a phase needs lives
// here.

#include "wp_types.h"

namespace wp {

// Maximum NEXT PIPE queue length any MVP level may use (fixed-size,
// docs/TECHNICAL_DESIGN.md section 4: avoid dynamic allocation).
constexpr int MAX_QUEUE = 32;

struct Level {
    const char *name;

    int sourceCol, sourceRow;
    Rotation sourceRotation;

    int targetCol, targetRow;
    Rotation targetRotation;

    // Delivered-volume objective (docs/GAMEPLAY.md section 18).
    int requiredVolume;

    // Maximum accumulated water loss tolerated by this phase.
    int maxLoss;

    // Programmed start delay before the source activates
    // (docs/GAMEPLAY.md section 15).
    uint32_t sourceDelayMs;

    // NEXT PIPE finite inventory (docs/GAMEPLAY.md section 16). All
    // queued pieces use PVC in the MVP (no material choice yet).
    PieceType queue[MAX_QUEUE];
    int queueLength;
};

// Phase 1 - Connect (docs/LEVEL_DESIGN.md section 12): one source, one
// target, straight/curve pieces only, deliver water to the target.
const Level &levelConnect();
const Level &levelDontSpill();
const Level &levelJunction();
const Level *levelByIndex(int phase);

}  // namespace wp
