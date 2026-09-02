#pragma once
// Lightweight game-local menus. They intentionally use the same software
// drawing primitives as gameplay so the game remains independent of LVGL.

#include "wp_progress.h"

class GfxCompat;

namespace wp {

enum class MenuScreen { Main, Phases, Scores };

struct MenuRect {
    int x, y, w, h;
    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

constexpr MenuRect MENU_CONTINUE = {180, 175, 440, 58};
constexpr MenuRect MENU_PHASES   = {180, 245, 210, 58};
constexpr MenuRect MENU_SCORES   = {410, 245, 210, 58};
constexpr MenuRect MENU_BACK     = {60, 400, 180, 54};

constexpr int PHASE_BUTTON_X = 150;
constexpr int PHASE_BUTTON_Y = 125;
constexpr int PHASE_BUTTON_W = 90;
constexpr int PHASE_BUTTON_H = 72;
constexpr int PHASE_BUTTON_GAP_X = 18;
constexpr int PHASE_BUTTON_GAP_Y = 18;
constexpr int PHASE_COLUMNS = 4;

constexpr MenuRect MENU_PHASE_BUTTONS[6] = {
    {150, 125, 90, 72}, {258, 125, 90, 72}, {366, 125, 90, 72}, {474, 125, 90, 72},
    {150, 215, 90, 72}, {258, 215, 90, 72},
};

void renderMenu(GfxCompat *g, MenuScreen screen, const ProgressData &progress);

}  // namespace wp
