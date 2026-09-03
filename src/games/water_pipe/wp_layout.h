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

// The current piece is the primary thing the player acts on, so it leads.
// HOLD sits directly beneath it with a connecting arrow, making the
// swap relationship visible instead of two unrelated boxes (docs/PIECES.md
// section 12: HOLD stores "the current piece").
constexpr Rect CURRENT_RECT = {HUD_X, 8, HUD_W, 56};
constexpr Rect HOLD_RECT = {HUD_X, 70, HUD_W, 40};
constexpr Rect HOLD_ARROW_RECT = {HUD_X, 64, HUD_W, 8};

// Upcoming pieces, previewed two ahead (docs/GAMEPLAY.md section 16).
constexpr Rect QUEUE_RECT[2] = {
    {HUD_X, 122, HUD_W, 36},
    {HUD_X, 164, HUD_W, 36},
};
constexpr Rect QUEUE_ARROW_RECT = {HUD_X, 158, HUD_W, 6};

// Material toggle. Purely a display choice today — it only starts
// mattering once pressure exists (docs/TECHNICAL_DESIGN.md section 25,
// step 13, deferred) — but the player can already pick it and see it
// reflected on the current-piece preview and on placed pipes.
constexpr int MATERIAL_GAP = 6;
constexpr Rect MATERIAL_PVC_RECT = {HUD_X, 210, (HUD_W - MATERIAL_GAP) / 2, 36};
constexpr Rect MATERIAL_CU_RECT = {
    HUD_X + (HUD_W - MATERIAL_GAP) / 2 + MATERIAL_GAP, 210, (HUD_W - MATERIAL_GAP) / 2, 36};

// Delivered water used to be a single flat bar. It's now a cascade of
// segments that darken as each one fills, then hands off to the next —
// like pouring water into a row of connected vessels — with a digit
// counter for the exact liter count underneath.
constexpr int CASCADE_SEGMENTS = 8;
constexpr Rect CASCADE_RECT = {HUD_X, 308, HUD_W, 18};

constexpr int LITER_DIGITS = 4;
constexpr int LITER_DIGIT_GAP = 3;
constexpr Rect LITER_DIGITS_AREA = {HUD_X, 330, HUD_W, 30};

inline Rect cascadeSegmentRect(int i) {
    const int gap = 3;
    const int w = (CASCADE_RECT.w - gap * (CASCADE_SEGMENTS - 1)) / CASCADE_SEGMENTS;
    return {CASCADE_RECT.x + i * (w + gap), CASCADE_RECT.y, w, CASCADE_RECT.h};
}

inline Rect literDigitRect(int i) {
    const int w = (LITER_DIGITS_AREA.w - LITER_DIGIT_GAP * (LITER_DIGITS - 1)) / LITER_DIGITS;
    return {LITER_DIGITS_AREA.x + i * (w + LITER_DIGIT_GAP), LITER_DIGITS_AREA.y, w, LITER_DIGITS_AREA.h};
}

constexpr Rect LOSS_METER_RECT = {HUD_X, 368, HUD_W, 18};

// Bottom controls: restart current phase, or save and leave to the Water
// Pipe main menu. The icons are deliberately large because the panel is
// touch-first and the HUD is narrow.
constexpr Rect RESTART_RECT = {HUD_X, 416, 52, 56};
constexpr Rect EXIT_RECT = {HUD_X + 60, 416, HUD_W - 60, 56};

inline Rect cellRect(int col, int row) {
    return {BOARD_ORIGIN_X + col * CELL_SIZE, BOARD_ORIGIN_Y + row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
}

}  // namespace wp
