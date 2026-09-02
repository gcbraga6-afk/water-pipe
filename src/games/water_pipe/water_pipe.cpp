#include "water_pipe.h"

#include "core/GfxCompat.h"
#include "core/TouchDriver.h"
#include "core/audio.h"
#include "wp_board.h"
#include "wp_input.h"
#include "wp_inventory.h"
#include "wp_levels.h"
#include "wp_render.h"
#include "wp_simulation.h"
#include "wp_time.h"

namespace WaterPipe {

namespace {

wp::Board board;
wp::SimState sim;
wp::WpInventory inventory;
wp::WpHudCache hudCache;
wp::Level currentLevel;
wp::Outcome lastOutcome = wp::Outcome::None;
uint32_t lastFrameMs = 0;
bool removeMode = false;

void loadLevel(const wp::Level &level) {
    currentLevel = level;
    board.loadLevel(level);
    wp::resetSimulation(sim, level);
    inventory.load(level);
    hudCache = wp::WpHudCache{};
    lastOutcome = wp::Outcome::None;
    removeMode = false;
    if (gfx) wp::renderInit(gfx, board);
    lastFrameMs = wp::nowMs();
}

wp::Outcome currentOutcome() {
    if (sim.victory) return wp::Outcome::Victory;
    if (wp::isDefeated(sim, inventory.empty())) return wp::Outcome::Defeat;
    return wp::Outcome::None;
}

}  // namespace

void begin() { loadLevel(wp::levelConnect()); }

bool loop() {
    const wp::WpAction action = wp::pollInput();

    if (action.type == wp::WpActionType::RestartTapped) {
        loadLevel(currentLevel);
        return false;
    }
    if (action.type == wp::WpActionType::RemoveModeToggled) {
        removeMode = !removeMode;
    }

    const wp::Outcome outcomeBeforeInput = currentOutcome();
    if (outcomeBeforeInput == wp::Outcome::None) {
        switch (action.type) {
            case wp::WpActionType::CellTapped: {
                wp::Cell &c = board.at(action.col, action.row);
                if (c.type == wp::PieceType::Empty) {
                    if (inventory.hand() != wp::PieceType::Empty &&
                        board.place(action.col, action.row, inventory.hand(), wp::Material::PVC)) {
                        inventory.consumeHand();
                        Audio::wpPlace();
                    }
                } else if (removeMode) {
                    if (board.remove(action.col, action.row)) Audio::wpRemove();
                } else if (board.canRotate(action.col, action.row)) {
                    board.rotate(action.col, action.row);
                    Audio::wpRotate();
                }
                break;
            }
            case wp::WpActionType::HoldTapped:
                inventory.swapHold();
                Audio::wpHold();
                break;
            default:
                break;
        }

        const uint32_t now = wp::nowMs();
        const uint32_t dt = now - lastFrameMs;
        lastFrameMs = now;
        const bool wasLeaking = sim.totalLoss > 0;
        wp::tickSimulation(sim, board, currentLevel, dt);
        if (!wasLeaking && sim.totalLoss > 0) Audio::wpLeak();
    } else {
        lastFrameMs = wp::nowMs();
    }

    const wp::Outcome outcome = currentOutcome();
    if (outcome != wp::Outcome::None && lastOutcome == wp::Outcome::None) {
        if (outcome == wp::Outcome::Victory) Audio::wpVictory();
        else Audio::wpDefeat();
    }
    lastOutcome = outcome;

    if (gfx) {
        wp::renderBoard(gfx, board, sim);
        wp::renderHud(gfx, inventory, sim, currentLevel, outcome, removeMode, hudCache);
    }

    return false;  // Water Pipe has no in-game exit gesture yet; the
                   // host's Games menu owns navigating away.
}

}  // namespace WaterPipe
