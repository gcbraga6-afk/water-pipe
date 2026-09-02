#pragma once
// Incremental renderer. Draws only dirty cells and only HUD regions whose
// underlying value changed. No text dependency is required by the current
// hardware compatibility layer.

#include "wp_board.h"
#include "wp_inventory.h"
#include "wp_levels.h"
#include "wp_simulation.h"

class GfxCompat;

namespace wp {

enum class Outcome { None, Victory, Defeat };

// Remembers the last value drawn in each HUD region so redraws only happen
// when something actually changed.
struct WpHudCache {
    PieceType hand = PieceType::Count;
    PieceType upcoming0 = PieceType::Count;
    PieceType upcoming1 = PieceType::Count;
    PieceType held = PieceType::Count;
    bool hasHeld = false;
    int lossBucket = -1;
    int deliveredBucket = -1;
    Outcome outcome = Outcome::None;
};

void renderInit(GfxCompat *g, Board &board);
void renderBoard(GfxCompat *g, Board &board, const SimState &sim);
void renderHud(GfxCompat *g, const WpInventory &inv, const SimState &sim, const Level &level,
               Outcome outcome, WpHudCache &cache);

}  // namespace wp
