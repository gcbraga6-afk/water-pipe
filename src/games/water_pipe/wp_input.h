#pragma once
// Converts raw touch into game actions (docs/TECHNICAL_DESIGN.md
// section 2, layer 3). Edge-triggered: TouchDriver::consumeTapInArea
// already guarantees one physical tap yields at most one action
// (docs/TECHNICAL_DESIGN.md section 15).

#include "wp_types.h"

namespace wp {

enum class WpActionType { None, CellTapped, HoldTapped, RestartTapped, RemoveModeToggled };

struct WpAction {
    WpActionType type = WpActionType::None;
    int col = -1;
    int row = -1;
};

// Polls the touch driver once and returns at most one action. Cheap
// enough to call every frame.
WpAction pollInput();

}  // namespace wp
