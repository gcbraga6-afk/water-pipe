#pragma once
// Persistent player progress for Water Pipe. The storage backend is kept out
// of the gameplay code so the native build can run without Arduino while the
// ESP32 firmware uses Preferences/NVS when available.

#include <cstdint>

#include "wp_board.h"
#include "wp_inventory.h"
#include "wp_simulation.h"

namespace wp {

constexpr int MAX_PHASES = 8;
constexpr int MAX_HIGHSCORES = 5;
constexpr uint32_t PROGRESS_MAGIC = 0x57505431u;  // "WPT1"
constexpr uint16_t PROGRESS_VERSION = 1;

struct HighScore {
    int score = 0;
    uint8_t phase = 0;
};

struct ResumeSnapshot {
    bool valid = false;
    uint8_t phase = 0;
    uint32_t elapsedMs = 0;
    Cell cells[BOARD_CELLS] = {};
    SimState sim = {};
    WpInventory::Snapshot inventory = {};
};

struct ProgressData {
    uint32_t magic = PROGRESS_MAGIC;
    uint16_t version = PROGRESS_VERSION;
    uint8_t unlockedPhase = 0;
    uint8_t stars[MAX_PHASES] = {};
    HighScore highScores[MAX_HIGHSCORES] = {};
    ResumeSnapshot resume = {};
};

class ProgressStore {
public:
    ProgressData load() const;
    bool save(const ProgressData &data) const;
    bool clearResume(ProgressData &data) const;

    static void recordPhaseResult(ProgressData &data, int phase, int score, int stars);
};

}  // namespace wp
