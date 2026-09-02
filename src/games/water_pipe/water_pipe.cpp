#include "water_pipe.h"

#include <algorithm>

#include "core/GfxCompat.h"
#include "core/TouchDriver.h"
#include "core/audio.h"
#include "wp_board.h"
#include "wp_input.h"
#include "wp_inventory.h"
#include "wp_levels.h"
#include "wp_menu.h"
#include "wp_progress.h"
#include "wp_render.h"
#include "wp_simulation.h"
#include "wp_time.h"

namespace WaterPipe {

namespace {

enum class GameMode { MainMenu, PhaseSelect, Scores, Playing };

wp::Board board;
wp::SimState sim;
wp::WpInventory inventory;
wp::WpHudCache hudCache;
wp::Level currentLevel;
wp::ProgressStore progressStore;
wp::ProgressData progress;
wp::MenuScreen menuScreen = wp::MenuScreen::Main;
wp::Outcome lastOutcome = wp::Outcome::None;
GameMode mode = GameMode::MainMenu;
uint32_t lastFrameMs = 0;
uint32_t phaseElapsedMs = 0;
uint32_t lastSaveMs = 0;

void resetProgressDefaults() {
    progress = wp::ProgressData{};
    progress.magic = wp::PROGRESS_MAGIC;
    progress.version = wp::PROGRESS_VERSION;
}

void saveResume() {
    progress.magic = wp::PROGRESS_MAGIC;
    progress.version = wp::PROGRESS_VERSION;
    progress.resume = wp::ResumeSnapshot{};
    progress.resume.valid = true;
    progress.resume.phase = 0;
    progress.resume.elapsedMs = phaseElapsedMs;
    for (int i = 0; i < wp::BOARD_CELLS; ++i) progress.resume.cells[i] = board.at(i);
    progress.resume.sim = sim;
    progress.resume.inventory = inventory.snapshot();
    progressStore.save(progress);
    lastSaveMs = wp::nowMs();
}

void loadLevel(const wp::Level &level, bool preserveResume = false) {
    currentLevel = level;
    board.loadLevel(level);
    wp::resetSimulation(sim, level);
    inventory.load(level);
    hudCache = wp::WpHudCache{};
    lastOutcome = wp::Outcome::None;
    phaseElapsedMs = 0;
    lastFrameMs = wp::nowMs();
    lastSaveMs = lastFrameMs;
    if (gfx) wp::renderInit(gfx, board);
    if (!preserveResume) saveResume();
}

bool restoreResume() {
    if (!progress.resume.valid || progress.resume.phase != 0) return false;
    loadLevel(wp::levelConnect(), true);
    for (int i = 0; i < wp::BOARD_CELLS; ++i) board.at(i) = progress.resume.cells[i];
    sim = progress.resume.sim;
    inventory.restore(progress.resume.inventory);
    phaseElapsedMs = progress.resume.elapsedMs;
    board.markAllDirty();
    hudCache = wp::WpHudCache{};
    lastOutcome = wp::Outcome::None;
    lastFrameMs = wp::nowMs();
    if (gfx) wp::renderInit(gfx, board);
    return true;
}

wp::Outcome currentOutcome() {
    if (sim.victory) return wp::Outcome::Victory;
    if (wp::isDefeated(sim, inventory.empty())) return wp::Outcome::Defeat;
    return wp::Outcome::None;
}

int phaseScore() {
    const int timePenalty = static_cast<int>(phaseElapsedMs / 1000u) * 3;
    const int lossPenalty = sim.totalLoss * 20;
    return std::max(100, 1000 - timePenalty - lossPenalty);
}

int phaseStars() {
    if (sim.totalLoss == 0 && phaseElapsedMs <= 20000u) return 3;
    if (sim.totalLoss <= 4 && phaseElapsedMs <= 45000u) return 2;
    return 1;
}

void recordVictoryOnce() {
    if (lastOutcome == wp::Outcome::Victory) return;
    wp::ProgressStore::recordPhaseResult(progress, 0, phaseScore(), phaseStars());
    progress.resume = wp::ResumeSnapshot{};
    progressStore.save(progress);
}

bool handleMenu() {
    if (!gfx) return false;

    if (menuScreen == wp::MenuScreen::Main) {
        if (progress.resume.valid &&
            TouchDriver::consumeTapInArea(wp::MENU_CONTINUE.x, wp::MENU_CONTINUE.y,
                                          wp::MENU_CONTINUE.x + wp::MENU_CONTINUE.w - 1,
                                          wp::MENU_CONTINUE.y + wp::MENU_CONTINUE.h - 1)) {
            if (restoreResume()) mode = GameMode::Playing;
            return false;
        }
        if (TouchDriver::consumeTapInArea(wp::MENU_PHASES.x, wp::MENU_PHASES.y,
                                          wp::MENU_PHASES.x + wp::MENU_PHASES.w - 1,
                                          wp::MENU_PHASES.y + wp::MENU_PHASES.h - 1)) {
            menuScreen = wp::MenuScreen::Phases;
            wp::renderMenu(gfx, menuScreen, progress);
            return false;
        }
        if (TouchDriver::consumeTapInArea(wp::MENU_SCORES.x, wp::MENU_SCORES.y,
                                          wp::MENU_SCORES.x + wp::MENU_SCORES.w - 1,
                                          wp::MENU_SCORES.y + wp::MENU_SCORES.h - 1)) {
            menuScreen = wp::MenuScreen::Scores;
            wp::renderMenu(gfx, menuScreen, progress);
            return false;
        }
        if (TouchDriver::consumeTapInArea(wp::MENU_EXIT.x, wp::MENU_EXIT.y,
                                          wp::MENU_EXIT.x + wp::MENU_EXIT.w - 1,
                                          wp::MENU_EXIT.y + wp::MENU_EXIT.h - 1)) {
            return true;
        }
        return false;
    }

    if (menuScreen == wp::MenuScreen::Phases) {
        for (int i = 0; i < 6; ++i) {
            const wp::MenuRect &r = wp::MENU_PHASE_BUTTONS[i];
            if (!TouchDriver::consumeTapInArea(r.x, r.y, r.x + r.w - 1, r.y + r.h - 1)) continue;
            if (i == 0 && i <= progress.unlockedPhase) {
                loadLevel(wp::levelConnect());
                mode = GameMode::Playing;
            }
            return false;
        }
        if (TouchDriver::consumeTapInArea(wp::MENU_BACK.x, wp::MENU_BACK.y,
                                          wp::MENU_BACK.x + wp::MENU_BACK.w - 1,
                                          wp::MENU_BACK.y + wp::MENU_BACK.h - 1)) {
            menuScreen = wp::MenuScreen::Main;
            wp::renderMenu(gfx, menuScreen, progress);
        }
        return false;
    }

    if (TouchDriver::consumeTapInArea(wp::MENU_BACK.x, wp::MENU_BACK.y,
                                      wp::MENU_BACK.x + wp::MENU_BACK.w - 1,
                                      wp::MENU_BACK.y + wp::MENU_BACK.h - 1)) {
        menuScreen = wp::MenuScreen::Main;
        wp::renderMenu(gfx, menuScreen, progress);
    }
    return false;
}

}  // namespace

void begin() {
    progress = progressStore.load();
    if (progress.magic != wp::PROGRESS_MAGIC || progress.version != wp::PROGRESS_VERSION) {
        resetProgressDefaults();
        progressStore.save(progress);
    }
    mode = GameMode::MainMenu;
    menuScreen = wp::MenuScreen::Main;
    lastFrameMs = wp::nowMs();
    if (gfx) wp::renderMenu(gfx, menuScreen, progress);
}

bool loop() {
    if (mode != GameMode::Playing) return handleMenu();

    const wp::WpAction action = wp::pollInput();

    if (action.type == wp::WpActionType::ExitTapped) {
        saveResume();
        return true;
    }

    if (action.type == wp::WpActionType::RestartTapped) {
        loadLevel(currentLevel);
        return false;
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
                } else if (board.canRotate(action.col, action.row)) {
                    board.rotate(action.col, action.row);
                    Audio::wpRotate();
                }
                break;
            }
            case wp::WpActionType::CellHeld:
                if (board.canRemove(action.col, action.row) && board.remove(action.col, action.row))
                    Audio::wpRemove();
                break;
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
        phaseElapsedMs += dt;
        const bool wasLeaking = sim.totalLoss > 0;
        wp::tickSimulation(sim, board, currentLevel, dt);
        if (!wasLeaking && sim.totalLoss > 0) Audio::wpLeak();

        if (now - lastSaveMs >= 10000u) saveResume();
    } else {
        lastFrameMs = wp::nowMs();
    }

    const wp::Outcome outcome = currentOutcome();
    if (outcome != wp::Outcome::None && lastOutcome == wp::Outcome::None) {
        if (outcome == wp::Outcome::Victory) {
            Audio::wpVictory();
            recordVictoryOnce();
        } else {
            Audio::wpDefeat();
            saveResume();
        }
    }
    lastOutcome = outcome;

    if (gfx) {
        wp::renderBoard(gfx, board, sim);
        wp::renderHud(gfx, inventory, sim, currentLevel, outcome, hudCache);
    }

    return false;
}

}  // namespace WaterPipe
