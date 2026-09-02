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

    // Source/target are fixed and not buildable-over.
    CHECK(!board.canPlace(level.sourceCol, level.sourceRow));
    CHECK(!board.canPlace(level.targetCol, level.targetRow));
    CHECK(!board.canRotate(level.sourceCol, level.sourceRow));
    CHECK(!board.canRemove(level.sourceCol, level.sourceRow));

    CHECK(board.canPlace(1, 2));
    CHECK(board.place(1, 2, PieceType::Straight, Material::PVC));
    CHECK(!board.place(1, 2, PieceType::Curve, Material::PVC));  // occupied now

    CHECK(board.canRotate(1, 2));
    CHECK(board.at(1, 2).rotation == Rotation::R0);
    CHECK(board.rotate(1, 2));
    CHECK(board.at(1, 2).rotation == Rotation::R90);

    // Dry pieces can be removed; the cell becomes placeable again.
    CHECK(board.canRemove(1, 2));
    CHECK(board.remove(1, 2));
    CHECK(board.canPlace(1, 2));

    std::puts("testBoardRules ok");
}

// Builds the reference solution for "Phase 1 - Connect" cell by cell,
// exactly as a player would via tap-to-place + tap-to-rotate, and
// verifies water reaches the target and the phase is won.
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

    // Row 2: four horizontal straights from the source.
    for (int c = 1; c <= 4; ++c) placeRotated(c, 2, PieceType::Straight, 1);
    // Turn down into row 3, then back to horizontal.
    placeRotated(5, 2, PieceType::Curve, 2);  // -> DOWN | LEFT
    placeRotated(5, 3, PieceType::Curve, 0);  // UP | RIGHT
    // Row 3: three horizontal straights into the target.
    for (int c = 6; c <= 8; ++c) placeRotated(c, 3, PieceType::Straight, 1);

    bool sawTargetReached = false;
    for (int tick = 0; tick < 2000 && !sim.victory; ++tick) {
        tickSimulation(sim, board, level, SIM_TIMESTEP_MS);
        sawTargetReached = sawTargetReached || sim.targetReached;
    }

    CHECK(sawTargetReached);
    CHECK(sim.victory);
    CHECK(sim.delivered >= level.requiredVolume);
    // The built path has no open ends, so nothing should have leaked.
    CHECK(sim.totalLoss == 0);
    CHECK(!isDefeated(sim, /*inventoryEmpty=*/true));

    std::puts("testSimulationVictory ok");
}

// A source left unconnected must leak immediately and, once the player
// has nothing left to place, the phase must be detected as defeated.
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
    CHECK(!isDefeated(sim, /*inventoryEmpty=*/false));  // pieces still available: not over yet
    CHECK(isDefeated(sim, /*inventoryEmpty=*/true));

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
    inv.swapHold();  // first press: stash hand, draw a new one
    CHECK(inv.hasHeld());
    CHECK(inv.held() == firstHand);
    CHECK(inv.hand() == level.queue[1]);

    inv.swapHold();  // second press: swap back
    CHECK(inv.hand() == firstHand);
    CHECK(inv.held() == level.queue[1]);

    int consumed = 0;
    while (!inv.empty()) {
        inv.consumeHand();
        if (inv.hand() == PieceType::Empty && inv.hasHeld()) inv.swapHold();
        if (++consumed > 1000) break;  // guard against an infinite loop bug
    }
    CHECK(inv.empty());

    std::puts("testInventory ok");
}

}  // namespace

int main() {
    GfxCompat stub(800, 480);  // unused by these tests, but keeps `gfx`
                               // symbol resolvable if any code path touches it.
    gfx = &stub;

    testPieceMasks();
    testBoardRules();
    testSimulationVictory();
    testSimulationDefeat();
    testInventory();

    std::printf("All %d checks passed.\n", g_checks);
    return 0;
}
