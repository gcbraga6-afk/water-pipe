#include "wp_board.h"

#include "wp_levels.h"
#include "wp_pieces.h"

namespace wp {

void Board::loadLevel(const Level &level) {
    for (int i = 0; i < BOARD_CELLS; ++i) cells_[i] = Cell{};

    Cell &source = at(level.sourceCol, level.sourceRow);
    source.type = PieceType::Source;
    source.rotation = level.sourceRotation;
    source.fixed = true;

    Cell &target = at(level.targetCol, level.targetRow);
    target.type = PieceType::Target;
    target.rotation = level.targetRotation;
    target.fixed = true;

    markAllDirty();
}

uint8_t Board::connectionMaskAt(int idx) const {
    const Cell &c = cells_[idx];
    if (c.type == PieceType::Empty) return 0;
    return connectionMask(c.type, c.rotation);
}

bool Board::canPlace(int col, int row) const {
    if (!inBounds(col, row)) return false;
    return at(col, row).type == PieceType::Empty;
}

bool Board::place(int col, int row, PieceType type, Material material) {
    if (!canPlace(col, row) || !isBuildable(type)) return false;
    Cell &c = at(col, row);
    c.type = type;
    c.rotation = Rotation::R0;
    c.material = material;
    c.fixed = false;
    c.volume = 0;
    c.dirty = true;
    return true;
}

bool Board::canRotate(int col, int row) const {
    if (!inBounds(col, row)) return false;
    const Cell &c = at(col, row);
    return c.type != PieceType::Empty && !c.fixed && c.volume == 0;
}

bool Board::rotate(int col, int row) {
    if (!canRotate(col, row)) return false;
    Cell &c = at(col, row);
    c.rotation = rotateCW(c.rotation);
    c.dirty = true;
    return true;
}

bool Board::canRemove(int col, int row) const {
    if (!inBounds(col, row)) return false;
    const Cell &c = at(col, row);
    return c.type != PieceType::Empty && !c.fixed && c.volume == 0;
}

bool Board::remove(int col, int row) {
    if (!canRemove(col, row)) return false;
    at(col, row) = Cell{};
    return true;
}

void Board::markAllDirty() {
    for (int i = 0; i < BOARD_CELLS; ++i) cells_[i].dirty = true;
}

}  // namespace wp
