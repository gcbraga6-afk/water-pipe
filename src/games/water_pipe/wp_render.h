#pragma once
// Incremental renderer (docs/TECHNICAL_DESIGN.md sections 16-17,
// docs/HARDWARE.md sections 2 and 5). Draws only dirty cells and only
// the HUD regions whose underlying value changed — no per-frame
// fillScreen, no text (gfx->print is a documented no-op today).

#include "wp_board.h"
#include "wp_inventory.h"
#include "wp_levels.h"
#include "wp_simulation.h"

class GfxCompat;

namespace wp {

enum class Outcome { None, Victory, Defeat };

// Remembers the last value drawn in each HUD region so redraws only
// happen when something actually changed.
struct WpHudCache {
    PieceType hand = PieceType::Count;  // sentinel: never a real value
    PieceType upcoming0 = PieceType::Count;
    PieceType upcoming1 = PieceType::Count;
    PieceType held = PieceType::Count;
    bool hasHeld = false;
    int removeMode = -1;  // -1 = unset, 0 = rotate mode, 1 = remove mode
    int lossBucket = -1;
    int deliveredBucket = -1;
    Outcome outcome = Outcome::None;
};

// Draws the static chrome once (board grid, HUD panel background) and
// marks the whole board dirty so the first renderBoard() call paints
// every cell.
void renderInit(GfxCompat *g, Board &board);

void renderBoard(GfxCompat *g, Board &board, const SimState &sim);
void renderHud(GfxCompat *g, const WpInventory &inv, const SimState &sim, const Level &level,
               Outcome outcome, bool removeMode, WpHudCache &cache);

}  // namespace wp
