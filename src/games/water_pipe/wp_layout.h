#pragma once
// Screen layout constants shared by the renderer and input mapper.
// The target display is the 800x480 CrowPanel Advance panel.

#include "wp_types.h"

namespace wp {

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 480;

// 10 x 7 board at 64px cells = 640 x 448. The board is vertically
// centered in the 480px display, leaving a small dark surround above/below.
constexpr int CELL_SIZE = 64;
constexpr int BOARD_ORIGIN_X = 8;
constexpr int BOARD_ORIGIN_Y = 16;
constexpr int BOARD_PIXEL_W = BOARD_COLS * CELL_SIZE;
constexpr int BOARD_PIXEL_H = BOARD_ROWS * CELL_SIZE;

// Compact dark HUD on the right.
constexpr int HUD_X = BOARD_ORIGIN_X + BOARD_PIXEL_W + 16;
constexpr int HUD_W = SCREEN_W - HUD_X - 8;

struct Rect {
    int x, y, w, h;
    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

// HOLD is the inventory hold slot. The first queue slot is the current
// piece, followed by the two upcoming pieces.
constexpr Rect HOLD_RECT = {HUD_X, 8, HUD_W, 76};
constexpr Rect QUEUE_RECT[3] = {
    {HUD_X, 92, HUD_W, 70},
    {HUD_X, 170, HUD_W, 60},
    {HUD_X, 238, HUD_W, 60},
};

constexpr Rect DELIVERED_METER_RECT = {HUD_X, 314, HUD_W, 24};
constexpr Rect LOSS_METER_RECT = {HUD_X, 348, HUD_W, 24};
constexpr Rect RESTART_RECT = {HUD_X, 416, HUD_W, 56};

inline Rect cellRect(int col, int row) {
    return {BOARD_ORIGIN_X + col * CELL_SIZE, BOARD_ORIGIN_Y + row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
}

}  // namespace wp
