#include "wp_simulation.h"

#include <algorithm>

namespace wp {

void resetSimulation(SimState &state, const Level &level) {
    state = SimState{};
    state.waterActive = false;
    state.delayRemainingMs = level.sourceDelayMs;
    state.warningStage = 0;
    state.sourceBlinkOn = false;
    state.blinkAccumulatorMs = 0;
}

static void stepOnce(SimState &s, Board &board, const Level &level) {
    if (s.victory) return;

    if (!s.waterActive) {
        if (s.delayRemainingMs > 0) {
            s.delayRemainingMs = (s.delayRemainingMs >= SIM_TIMESTEP_MS) ? s.delayRemainingMs - SIM_TIMESTEP_MS : 0;
            s.blinkAccumulatorMs += SIM_TIMESTEP_MS;
            if (s.blinkAccumulatorMs >= 250) {
                s.blinkAccumulatorMs = 0;
                s.sourceBlinkOn = !s.sourceBlinkOn;
            }
            if (s.delayRemainingMs <= 3000 && s.warningStage == 0) s.warningStage = 3;
            else if (s.delayRemainingMs <= 2000 && s.warningStage == 3) s.warningStage = 2;
            else if (s.delayRemainingMs <= 1000 && s.warningStage == 2) s.warningStage = 1;
            return;
        }
        s.waterActive = true;
        s.warningStage = 4;
        s.sourceBlinkOn = true;
    }

    const int sourceIdx = Board::index(level.sourceCol, level.sourceRow);
    if (!s.hasStarted) {
        s.hasStarted = true;
        s.reached[sourceIdx] = true;
        board.at(sourceIdx).dirty = true;
    }

    // Progressive filling of every cell already reached by water
    // (docs/WATER_SYSTEM.md section 2).
    for (int idx = 0; idx < BOARD_CELLS; ++idx) {
        if (!s.reached[idx]) continue;
        Cell &c = board.at(idx);
        if (c.volume < PIPE_CAPACITY) {
            c.volume = static_cast<uint8_t>(std::min<int>(PIPE_CAPACITY, c.volume + FILL_RATE_PER_TICK));
            c.dirty = true;
        }
    }

    // Re-evaluate connectivity and open-end loss from scratch every tick
    // so late placements can reconnect a leaking port
    // (docs/GAMEPLAY.md section 6: "Disconnected pipes remain inactive
    // until connected to an active hydraulic system").
    bool newlyReachedAny = false;
    for (int idx = 0; idx < BOARD_CELLS; ++idx) {
        if (!s.reached[idx]) continue;
        const uint8_t mask = board.connectionMaskAt(idx);
        if (mask == 0) continue;

        const int col = idx % BOARD_COLS;
        const int row = idx / BOARD_COLS;
        bool cellLeaking = false;

        static constexpr uint8_t kBits[4] = {DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT};
        for (uint8_t bit : kBits) {
            if (!(mask & bit)) continue;
            int dx, dy;
            dirDelta(bit, dx, dy);
            const int nc = col + dx;
            const int nr = row + dy;
            if (!Board::inBounds(nc, nr)) {
                cellLeaking = true;
                continue;
            }
            const int nIdx = Board::index(nc, nr);
            if (board.at(nIdx).type == PieceType::Empty) {
                cellLeaking = true;
                continue;
            }
            const uint8_t nMask = board.connectionMaskAt(nIdx);
            if (!(nMask & oppositeDir(bit))) {
                cellLeaking = true;
                continue;
            }
            if (!s.reached[nIdx]) {
                s.reached[nIdx] = true;
                newlyReachedAny = true;
                board.at(nIdx).dirty = true;
                if (board.at(nIdx).type == PieceType::Target) s.targetReached = true;
            }
        }

        if (cellLeaking != s.leaking[idx]) board.at(idx).dirty = true;
        s.leaking[idx] = cellLeaking;
        if (cellLeaking) s.totalLoss += LEAK_RATE_PER_TICK;
    }
    s.lastTickProgressed = newlyReachedAny;

    if (s.targetReached) {
        s.delivered = std::min(level.requiredVolume, s.delivered + DELIVER_RATE_PER_TICK);
        board.at(level.targetCol, level.targetRow).dirty = true;
        if (s.delivered >= level.requiredVolume) s.victory = true;
    }
}

void tickSimulation(SimState &state, Board &board, const Level &level, uint32_t dtMs) {
    if (state.victory) return;
    state.accumulatorMs += dtMs;
    while (state.accumulatorMs >= SIM_TIMESTEP_MS) {
        state.accumulatorMs -= SIM_TIMESTEP_MS;
        stepOnce(state, board, level);
        if (state.victory) break;
    }
}

}  // namespace wp
