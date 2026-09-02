#pragma once
// Converts raw touch into game actions. A short tap on a placed pipe
// rotates it; a press-and-hold removes it once the hold threshold is met.

#include "wp_types.h"

namespace wp {

enum class WpActionType {
    None,
    CellTapped,
    CellHeld,
    HoldTapped,
    RestartTapped,
    ExitTapped,
};

struct WpAction {
    WpActionType type = WpActionType::None;
    int col = -1;
    int row = -1;
};

// Polls the touch driver once and returns at most one action.
WpAction pollInput();

}  // namespace wp
