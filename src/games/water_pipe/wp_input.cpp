#include "wp_input.h"

#include "core/TouchDriver.h"
#include "wp_layout.h"

namespace wp {

WpAction pollInput() {
    if (TouchDriver::consumeTapInArea(HOLD_RECT.x, HOLD_RECT.y, HOLD_RECT.x + HOLD_RECT.w - 1,
                                       HOLD_RECT.y + HOLD_RECT.h - 1)) {
        return {WpActionType::HoldTapped, -1, -1};
    }
    if (TouchDriver::consumeTapInArea(RESTART_RECT.x, RESTART_RECT.y, RESTART_RECT.x + RESTART_RECT.w - 1,
                                       RESTART_RECT.y + RESTART_RECT.h - 1)) {
        return {WpActionType::RestartTapped, -1, -1};
    }
    if (TouchDriver::consumeTapInArea(REMOVE_TOGGLE_RECT.x, REMOVE_TOGGLE_RECT.y,
                                       REMOVE_TOGGLE_RECT.x + REMOVE_TOGGLE_RECT.w - 1,
                                       REMOVE_TOGGLE_RECT.y + REMOVE_TOGGLE_RECT.h - 1)) {
        return {WpActionType::RemoveModeToggled, -1, -1};
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
