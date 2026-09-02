#include "wp_pieces.h"

namespace wp {

uint8_t baseMask(PieceType type) {
    switch (type) {
        case PieceType::Straight: return DIR_UP | DIR_DOWN;
        case PieceType::Curve:    return DIR_UP | DIR_RIGHT;
        case PieceType::T:        return DIR_UP | DIR_RIGHT | DIR_DOWN;
        case PieceType::Cross:    return DIR_ALL;
        case PieceType::Cap:      return DIR_UP;
        case PieceType::Source:   return DIR_UP;
        case PieceType::Target:   return DIR_UP;
        default:                  return 0;
    }
}

static uint8_t rotateMaskCW(uint8_t mask) {
    uint8_t out = 0;
    if (mask & DIR_UP)    out |= DIR_RIGHT;
    if (mask & DIR_RIGHT) out |= DIR_DOWN;
    if (mask & DIR_DOWN)  out |= DIR_LEFT;
    if (mask & DIR_LEFT)  out |= DIR_UP;
    return out;
}

uint8_t connectionMask(PieceType type, Rotation rotation) {
    uint8_t mask = baseMask(type);
    for (int i = 0; i < static_cast<int>(rotation); ++i) mask = rotateMaskCW(mask);
    return mask;
}

bool isBuildable(PieceType type) {
    switch (type) {
        case PieceType::Straight:
        case PieceType::Curve:
        case PieceType::T:
        case PieceType::Cross:
        case PieceType::Cap:
            return true;
        default:
            return false;
    }
}

}  // namespace wp
