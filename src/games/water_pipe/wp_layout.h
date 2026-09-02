#pragma once
// Screen layout constants shared by the renderer and the input mapper.
// The target display is the 800x480 CrowPanel Advance panel.

#include "wp_types.h"

namespace wp {

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 480;

// Keep the physical 64px cells. With 10 x 7 cells the board is 640 x 448
// and is vertically centered with a 16px top/bottom margin.
constexpr int CELL_SIZE = 64;
constexpr int BOARD_ORIGIN_X = 8;
constexpr int BOARD_ORIGIN_Y = 16;
constexpr int BOARD_PIXEL_W = BOARD_COLS * CELL_SIZE;
constexpr int BOARD_PIXEL_H = BOARD_ROWS * CELL_SIZE;

// The remaining right-hand strip is a compact dark HUD. The dark surround
// intentionally continues around the whole board so there is no accidental
// white band below it.
constexpr int HUD_X = BOARD_ORIGIN_X + BOARD_PIXEL_W + 16;
constexpr int HUD_W = SCREEN_W - HUD_X - 8;

struct Rect {
    int x, y, w, h;
    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

// HUD regions, top to bottom. The current piece gets a larger, highlighted
// slot; the two upcoming pieces are compact. HOLD is separate and visually
// emphasized so the touch targets are easier to understand.
constexpr Rect HOLD_RECT = {HUD_X, 8, HUD_W, 76};
constexpr Rect QUEUE_RECT[3] = {
    {HUD_X, 92, HUD_W, 70},
    {HUD_X, 170, HUD_W, 60},
    {HUD_X, 238, HUD_W, 60},
};

constexpr Rect REMOVE_TOGGLE_RECT = {HUD_X, 306, HUD_W, 34};
constexpr Rect DELIVERED_METER_RECT = {HUD_X, 348, HUD_W, 24};
constexpr Rect LOSS_METER_RECT = {HUD_X, 380, HUD_W, 24};
constexpr Rect RESTART_RECT = {HUD_X, 424, HUD_W, 48};

inline Rect cellRect(int col, int row) {
    return {BOARD_ORIGIN_X + col * CELL_SIZE, BOARD_ORIGIN_Y + row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
}

}  // namespace wp
