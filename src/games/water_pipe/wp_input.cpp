#include "wp_input.h"

#include "core/TouchDriver.h"
#include "wp_board.h"
#include "wp_layout.h"
#include "wp_time.h"

namespace wp {

namespace {
constexpr uint32_t HOLD_MS = 450;
bool fingerDown = false;
bool longPressFired = false;
bool suppressNextTap = false;
uint32_t fingerDownMs = 0;
int fingerCol = -1;
int fingerRow = -1;

bool pointInBoard(int x, int y, int &col, int &row) {
    if (x < BOARD_ORIGIN_X || x >= BOARD_ORIGIN_X + BOARD_PIXEL_W ||
        y < BOARD_ORIGIN_Y || y >= BOARD_ORIGIN_Y + BOARD_PIXEL_H) {
        return false;
    }
    col = (x - BOARD_ORIGIN_X) / CELL_SIZE;
    row = (y - BOARD_ORIGIN_Y) / CELL_SIZE;
    return Board::inBounds(col, row);
}
}  // namespace

WpAction pollInput() {
    // Track a real finger while it remains down. The short tap is deliberately
    // delayed until release so a long press cannot rotate the same pipe first.
    if (TouchDriver::rawTouched()) {
        const int x = TouchDriver::rawX();
        const int y = TouchDriver::rawY();
        int col = -1;
        int row = -1;
        const bool onBoard = pointInBoard(x, y, col, row);

        if (!fingerDown) {
            fingerDown = true;
            longPressFired = false;
            fingerDownMs = nowMs();
            fingerCol = col;
            fingerRow = row;
        }

        if (onBoard && !longPressFired && nowMs() - fingerDownMs >= HOLD_MS) {
            longPressFired = true;
            return {WpActionType::CellHeld, fingerCol, fingerRow};
        }
        return {};
    }

    if (fingerDown) {
        suppressNextTap = longPressFired;
        fingerDown = false;
        longPressFired = false;
        fingerCol = -1;
        fingerRow = -1;
    }

    if (suppressNextTap) {
        // Consume the release edge, but deliberately emit no second action.
        TouchDriver::consumeTapInArea(0, 0, SCREEN_W - 1, SCREEN_H - 1);
        suppressNextTap = false;
        return {};
    }

    if (TouchDriver::consumeTapInArea(HOLD_RECT.x, HOLD_RECT.y,
                                      HOLD_RECT.x + HOLD_RECT.w - 1,
                                      HOLD_RECT.y + HOLD_RECT.h - 1)) {
        return {WpActionType::HoldTapped, -1, -1};
    }
    if (TouchDriver::consumeTapInArea(RESTART_RECT.x, RESTART_RECT.y,
                                      RESTART_RECT.x + RESTART_RECT.w - 1,
                                      RESTART_RECT.y + RESTART_RECT.h - 1)) {
        return {WpActionType::RestartTapped, -1, -1};
    }
    if (TouchDriver::consumeTapInArea(EXIT_RECT.x, EXIT_RECT.y,
                                      EXIT_RECT.x + EXIT_RECT.w - 1,
                                      EXIT_RECT.y + EXIT_RECT.h - 1)) {
        return {WpActionType::ExitTapped, -1, -1};
    }
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            const Rect r = cellRect(col, row);
            if (TouchDriver::consumeTapInArea(r.x, r.y, r.x + r.w - 1, r.y + r.h - 1)) {
                return {WpActionType::CellTapped, col, row};
            }
        }
    }
    return {};
}

}  // namespace wp
