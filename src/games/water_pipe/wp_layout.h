#pragma once
// Screen layout constants shared by the renderer and the input mapper
// (docs/HARDWARE.md section 1: 800x480 panel).

#include "wp_types.h"

namespace wp {

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 480;

// 10 x 6 board at 64px cells = 640 x 384, top-left aligned with a small
// margin. The remaining right-hand strip is the HUD.
constexpr int CELL_SIZE = 64;
constexpr int BOARD_ORIGIN_X = 8;
constexpr int BOARD_ORIGIN_Y = 8;
constexpr int BOARD_PIXEL_W = BOARD_COLS * CELL_SIZE;
constexpr int BOARD_PIXEL_H = BOARD_ROWS * CELL_SIZE;

constexpr int HUD_X = BOARD_ORIGIN_X + BOARD_PIXEL_W + 16;
constexpr int HUD_W = SCREEN_W - HUD_X - 8;

struct Rect {
    int x, y, w, h;
    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

// HUD regions, top to bottom: HOLD slot, current + 2 preview queue
// slots, loss meter, restart button (docs/GAMEPLAY.md section 16).
constexpr Rect HOLD_RECT = {HUD_X, 8, HUD_W, 72};
constexpr Rect QUEUE_RECT[3] = {
    {HUD_X, 92, HUD_W, 64},
    {HUD_X, 164, HUD_W, 52},
    {HUD_X, 224, HUD_W, 44},
};
// Toggles whether tapping an existing piece rotates it (default) or
// removes it when dry (docs/PIECES.md section 10). A dedicated control
// keeps single-tap semantics unambiguous, per the documented touch API
// (docs/HARDWARE.md section 3: one point, no gestures).
constexpr Rect REMOVE_TOGGLE_RECT = {HUD_X, 270, HUD_W, 26};

constexpr Rect DELIVERED_METER_RECT = {HUD_X, 300, HUD_W, 24};
constexpr Rect LOSS_METER_RECT = {HUD_X, 360, HUD_W, 24};
constexpr Rect RESTART_RECT = {HUD_X, 432, HUD_W, 40};

inline Rect cellRect(int col, int row) {
    return {BOARD_ORIGIN_X + col * CELL_SIZE, BOARD_ORIGIN_Y + row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
}

}  // namespace wp
