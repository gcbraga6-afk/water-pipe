#pragma once
// Board representation and construction rules (docs/GAMEPLAY.md sections
// 2-4, docs/PIECES.md sections 9-10). Pure logic, no rendering/hardware
// dependency.

#include "wp_types.h"

namespace wp {

struct Level;  // fwd decl, see wp_levels.h

class Board {
public:
    void loadLevel(const Level &level);

    static bool inBounds(int col, int row) {
        return col >= 0 && col < BOARD_COLS && row >= 0 && row < BOARD_ROWS;
    }
    static int index(int col, int row) { return row * BOARD_COLS + col; }

    Cell &at(int col, int row) { return cells_[index(col, row)]; }
    const Cell &at(int col, int row) const { return cells_[index(col, row)]; }
    Cell &at(int idx) { return cells_[idx]; }
    const Cell &at(int idx) const { return cells_[idx]; }

    uint8_t connectionMaskAt(int idx) const;
    uint8_t connectionMaskAt(int col, int row) const { return connectionMaskAt(index(col, row)); }

    // Construction rules (docs/GAMEPLAY.md section 3-4, docs/PIECES.md
    // section 9-10). Each returns false and does nothing when the rule
    // forbids the action.
    bool canPlace(int col, int row) const;
    bool place(int col, int row, PieceType type, Material material);
    bool canRotate(int col, int row) const;
    bool rotate(int col, int row);
    bool canRemove(int col, int row) const;
    bool remove(int col, int row);

    void markAllDirty();

private:
    Cell cells_[BOARD_CELLS];
};

}  // namespace wp
