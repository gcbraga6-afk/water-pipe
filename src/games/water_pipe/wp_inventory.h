#pragma once
// NEXT PIPE inventory: a finite queue plus one Tetris-style HOLD slot
// (docs/GAMEPLAY.md section 16, docs/PIECES.md section 12).

#include "wp_levels.h"
#include "wp_types.h"

namespace wp {

class WpInventory {
public:
    void load(const Level &level);

    // The piece that placement acts on right now.
    PieceType hand() const { return hand_; }
    // 0 = next piece after hand, 1 = the one after that
    // ("current piece plus two future pieces are visible").
    PieceType peekUpcoming(int aheadIndex) const;

    PieceType held() const { return held_; }
    bool hasHeld() const { return hasHeld_; }

    // Nothing left to place at all: no hand piece and no usable held one.
    bool empty() const;

    // Call after a successful placement of hand().
    void consumeHand();

    // Tetris-style hold: first press stores the hand piece and draws a
    // new one; every press after that swaps hand and held.
    void swapHold();

private:
    PieceType draw();

    PieceType queue_[MAX_QUEUE] = {};
    int length_ = 0;
    int nextIndex_ = 0;
    PieceType hand_ = PieceType::Empty;
    PieceType held_ = PieceType::Empty;
    bool hasHeld_ = false;
};

}  // namespace wp
