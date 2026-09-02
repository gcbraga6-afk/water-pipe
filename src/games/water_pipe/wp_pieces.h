#pragma once
// Piece shape/connection definitions (docs/PIECES.md).

#include "wp_types.h"

namespace wp {

// Base (Rotation::R0) connection mask for each piece type.
// Straight: vertical by default. Curve: connects up+right by default.
// T: three ports, missing LEFT by default. Cross: all four.
// Cap/Source/Target: a single port, oriented by rotation at placement.
uint8_t baseMask(PieceType type);

// Connection mask of a piece after applying its rotation.
uint8_t connectionMask(PieceType type, Rotation rotation);

// Whether the player can place/rotate/remove this type directly
// (Source/Target are fixed level infrastructure, never buildable).
bool isBuildable(PieceType type);

}  // namespace wp
