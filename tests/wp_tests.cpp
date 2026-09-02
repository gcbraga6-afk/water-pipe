// Native, hardware-free logic tests for Water Pipe. No test framework
// dependency: each check aborts the process on failure so a green run
// means every assertion held.

#include <cstdio>
#include <cstdlib>

#include "core/GfxCompat.h"
#include "games/water_pipe/wp_board.h"
#include "games/water_pipe/wp_inventory.h"
#include "games/water_pipe/wp_levels.h"
#include "games/water_pipe/wp_pieces.h"
#include "games/water_pipe/wp_progress.h"
#include "games/water_pipe/wp_simulation.h"

namespace {

int g_checks = 0;

#define CHECK(cond)                                                                 \
    do {                                                                            \
        ++g_checks;                                                                 \
        if (!(cond)) {                                                              \
            std::fprintf(stderr, "FAILED: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
            std::abort();                                                           \
        }                                                                           \
    } while (0)

void testPieceMasks() {
    using namespace wp;
    CHECK(connectionMask(PieceType::Straight, Rotation::R0) == (DIR_UP | DIR_DOWN));
    CHECK(connectionMask(PieceType::Straight, Rotation::R90) == (DIR_LEFT | DIR_RIGHT));
    CHECK(connectionMask(PieceType::Straight, Rotation::R180) == (DIR_UP | DIR_DOWN));
    CHECK(connectionMask(PieceType::Curve, Rotation::R0) == (DIR_UP | DIR_RIGHT));
    CHECK(connectionMask(PieceType::Curve, Rotation::R180) == (DIR_DOWN | DIR_LEFT));
    CHECK(connectionMask(PieceType::Cross, Rotation::R0) == DIR_ALL);
    CHECK(connectionMask(PieceType::Cross, Rotation::R90) == DIR_ALL);
    CHECK(connectionMask(PieceType::Cap, Rotation::R270) == DIR_LEFT);
    CHECK(isBuildable(PieceType::Straight));
    CHECK(!isBuildable(PieceType::Source));
    CHECK(!isBuildable(PieceType::Target));
    std::puts("testPieceMasks ok");
}

void testBoardRules() {
    using namespace wp;
    const Level &level = levelConnect();
    Board board;
    board.loadLevel(level);

    CHECK(!board.canPlace(level.sourceCol, level.sourceRow));
    CHECK(!board.canPlace(level.targetCol, level.targetRow));
    CHECK(!board.canRotate(level.sourceCol, level.sourceRow));
    CHECK(!board.canRemove(level.sourceCol, level.sourceRow));

    CHECK(board.canPlace(1, 2));
    CHECK(board.place(1, 2, PieceType::Straight, Material::PVC));
    CHECK(!board.place(1, 2, PieceType::Curve, Material::PVC));

    CHECK(board.canRotate(1, 2));
    CHECK(board.rotate(1, 2));
    CHECK(board.at(1, 2).rotation == Rotation::R90);

    // A pipe containing water is neither rotatable nor removable.
    board.at(1, 2).volume = 1;
    CHECK(!board.canRotate(1, 2));
    CHECK(!board.rotate(1, 2));
    CHECK(!board.canRemove(1, 2));
    board.at(1, 2).volume = 0;

    CHECK(board.canRemove(1, 2));
    CHECK(board.remove(1, 2));
    CHECK(board.canPlace(1, 2));

    std::puts("testBoardRules ok");
}

void testSimulationVictory() {
    using namespace wp;
    const Level &level = levelConnect();
    Board board;
    board.loadLevel(level);
    SimState sim;
    resetSimulation(sim, level);

    auto placeRotated = [&](int c, int r, PieceType type, int rotations) {
        CHECK(board.place(c, r, type, Material::PVC));
        for (int i = 0; i < rotations; ++i) CHECK(board.rotate(c, r));
    };

    for (int c = 1; c <= 4; ++c) placeRotated(c, 2, PieceType::Straight, 1);
    placeRotated(5, 2, PieceType::Curve, 2);
    placeRotated(5, 3, PieceType::Curve, 0);
    for (int c = 6; c <= 8; ++c) placeRotated(c, 3, PieceType::Straight, 1);

    bool sawTargetReached = false;
    for (int tick = 0; tick < 2000 && !sim.victory; ++tick) {
        tickSimulation(sim, board, level, SIM_TIMESTEP_MS);
        sawTargetReached = sawTargetReached || sim.targetReached;
    }

    CHECK(sawTargetReached);
    CHECK(sim.victory);
    CHECK(sim.delivered >= level.requiredVolume);
    CHECK(sim.totalLoss == 0);
    CHECK(!isDefeated(sim, true));

    std::puts("testSimulationVictory ok");
}

void testSimulationDefeat() {
    using namespace wp;
    const Level &level = levelConnect();
    Board board;
    board.loadLevel(level);
    SimState sim;
    resetSimulation(sim, level);

    for (int tick = 0; tick < 50; ++tick) tickSimulation(sim, board, level, SIM_TIMESTEP_MS);

    CHECK(sim.totalLoss > 0);
    CHECK(!sim.targetReached);
    CHECK(!sim.victory);
    CHECK(!isDefeated(sim, false));
    CHECK(isDefeated(sim, true));

    std::puts("testSimulationDefeat ok");
}

void testInventory() {
    using namespace wp;
    const Level &level = levelConnect();
    WpInventory inv;
    inv.load(level);

    CHECK(inv.hand() == level.queue[0]);
    CHECK(inv.peekUpcoming(0) == level.queue[1]);
    CHECK(inv.peekUpcoming(1) == level.queue[2]);
    CHECK(!inv.hasHeld());

    const PieceType firstHand = inv.hand();
    inv.swapHold();
    CHECK(inv.hasHeld());
    CHECK(inv.held() == firstHand);
    CHECK(inv.hand() == level.queue[1]);

    // HOLD cannot be spammed: the player must place the current piece first.
    const PieceType blockedHand = inv.hand();
    inv.swapHold();
    CHECK(inv.hand() == blockedHand);
    CHECK(inv.held() == firstHand);

    inv.consumeHand();
    CHECK(inv.hand() == level.queue[2]);
    inv.swapHold();
    CHECK(inv.hand() == firstHand);
    CHECK(inv.held() == level.queue[2]);

    WpInventory::Snapshot snapshot = inv.snapshot();
    WpInventory restored;
    restored.restore(snapshot);
    CHECK(restored.hand() == inv.hand());
    CHECK(restored.held() == inv.held());
    CHECK(restored.hasHeld() == inv.hasHeld());

    std::puts("testInventory ok");
}

void testProgress() {
    using namespace wp;
    ProgressData p{};
    CHECK(p.magic == PROGRESS_MAGIC);
    CHECK(p.version == PROGRESS_VERSION);
    CHECK(p.unlockedPhase == 0);

    ProgressStore::recordPhaseResult(p, 0, 820, 2);
    CHECK(p.stars[0] == 2);
    CHECK(p.highScores[0].score == 820);
    CHECK(p.highScores[0].phase == 0);
    CHECK(p.unlockedPhase == 1);

    ProgressStore::recordPhaseResult(p, 0, 950, 3);
    CHECK(p.stars[0] == 3);
    CHECK(p.highScores[0].score == 950);
    CHECK(p.highScores[1].score == 820);

    std::puts("testProgress ok");
}

}  // namespace

int main() {
    GfxCompat stub(800, 480);
    gfx = &stub;

    testPieceMasks();
    testBoardRules();
    testSimulationVictory();
    testSimulationDefeat();
    testInventory();
    testProgress();

    std::printf("All %d checks passed.\n", g_checks);
    return 0;
}
