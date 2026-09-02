#pragma once
// Core data types for Water Pipe. No hardware or rendering dependency —
// see docs/TECHNICAL_DESIGN.md section 2 ("the hydraulic simulation must
// not depend on rendering code").

#include <cstdint>

namespace wp {

// MVP board size (docs/GAMEPLAY.md section 2, docs/TECHNICAL_DESIGN.md
// section 4).
constexpr int BOARD_COLS = 10;
constexpr int BOARD_ROWS = 6;
constexpr int BOARD_CELLS = BOARD_COLS * BOARD_ROWS;

// Direction bitmask (docs/TECHNICAL_DESIGN.md section 5).
enum Direction : uint8_t {
    DIR_UP = 1,
    DIR_RIGHT = 2,
    DIR_DOWN = 4,
    DIR_LEFT = 8,
};
constexpr uint8_t DIR_ALL = DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT;

inline uint8_t oppositeDir(uint8_t singleBit) {
    // Rotating a single bit by 180 degrees within the 4-bit ring.
    return static_cast<uint8_t>(((singleBit << 2) | (singleBit >> 2)) & 0xF);
}

inline void dirDelta(uint8_t singleBit, int &dx, int &dy) {
    switch (singleBit) {
        case DIR_UP:    dx = 0;  dy = -1; break;
        case DIR_RIGHT: dx = 1;  dy = 0;  break;
        case DIR_DOWN:  dx = 0;  dy = 1;  break;
        case DIR_LEFT:  dx = -1; dy = 0;  break;
        default:        dx = 0;  dy = 0;  break;
    }
}

// Buildable + fixed piece types (docs/PIECES.md, MVP catalog subset per
// docs/TECHNICAL_DESIGN.md section 24).
enum class PieceType : uint8_t {
    Empty = 0,
    Straight,
    Curve,
    T,
    Cross,
    Cap,
    Source,  // fixed, pre-built (docs/GAMEPLAY.md section 2)
    Target,  // fixed, pre-built
    Count,
};

// The five pieces a player can actually place from the queue.
constexpr PieceType kBuildablePieces[] = {
    PieceType::Straight, PieceType::Curve, PieceType::T,
    PieceType::Cross,    PieceType::Cap,
};

enum class Material : uint8_t {
    None = 0,
    PVC,
    Copper,
};

enum class Rotation : uint8_t { R0 = 0, R90 = 1, R180 = 2, R270 = 3 };

inline Rotation rotateCW(Rotation r) {
    return static_cast<Rotation>((static_cast<int>(r) + 1) % 4);
}

struct MaterialInfo {
    int cost;
    int pressureLimit;  // reserved for a future pressure system (docs/WATER_SYSTEM.md section 9)
};

inline const MaterialInfo &materialInfo(Material m) {
    static constexpr MaterialInfo kInfo[3] = {
        {0, 0},      // None
        {10, 700},   // PVC   (docs/TECHNICAL_DESIGN.md section 11)
        {25, 1000},  // Copper
    };
    return kInfo[static_cast<int>(m)];
}

// Water/timing defaults (docs/TECHNICAL_DESIGN.md section 12).
constexpr int PIPE_CAPACITY = 10;
constexpr int SIM_TIMESTEP_MS = 100;
constexpr int FILL_RATE_PER_TICK = 3;
constexpr int LEAK_RATE_PER_TICK = 2;
constexpr int DELIVER_RATE_PER_TICK = 2;

struct Cell {
    PieceType type = PieceType::Empty;
    Rotation rotation = Rotation::R0;
    Material material = Material::None;
    bool fixed = false;   // pre-built infrastructure: cannot rotate/remove
    uint8_t volume = 0;   // 0..PIPE_CAPACITY, visual fill level
    bool dirty = true;    // needs a redraw
};

}  // namespace wp
