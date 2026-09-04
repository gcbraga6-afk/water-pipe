#include "wp_levels.h"

namespace wp {
namespace {

Level makeConnect() {
    Level l{};
    l.name = "Phase 1 - Connect";
    l.sourceCol = 0; l.sourceRow = 3; l.sourceRotation = Rotation::R90;
    l.targetCol = 9; l.targetRow = 3; l.targetRotation = Rotation::R270;
    l.requiredVolume = 40;
    l.maxLoss = 60;
    l.sourceDelayMs = 10000;
    const PieceType q[] = {
        PieceType::Straight, PieceType::Straight, PieceType::Straight, PieceType::Straight,
        PieceType::Straight, PieceType::Straight, PieceType::Straight, PieceType::Straight,
    };
    l.queueLength = 8;
    for (int i = 0; i < l.queueLength; ++i) l.queue[i] = q[i];
    return l;
}

Level makeDontSpill() {
    Level l{};
    l.name = "Phase 2 - Don't Spill";
    l.sourceCol = 0; l.sourceRow = 2; l.sourceRotation = Rotation::R90;
    l.targetCol = 9; l.targetRow = 4; l.targetRotation = Rotation::R270;
    l.requiredVolume = 40;
    l.maxLoss = 40;
    l.sourceDelayMs = 8000;
    const PieceType q[] = {
        PieceType::Straight, PieceType::Straight, PieceType::Straight, PieceType::Straight,
        PieceType::Curve, PieceType::Straight, PieceType::Curve,
        PieceType::Straight, PieceType::Straight, PieceType::Straight,
    };
    l.queueLength = 10;
    for (int i = 0; i < l.queueLength; ++i) l.queue[i] = q[i];
    return l;
}

Level makeJunction() {
    Level l{};
    l.name = "Phase 3 - Junction";
    l.sourceCol = 0; l.sourceRow = 3; l.sourceRotation = Rotation::R90;
    l.targetCol = 9; l.targetRow = 3; l.targetRotation = Rotation::R270;
    l.requiredVolume = 40;
    l.maxLoss = 0;
    l.sourceDelayMs = 7000;
    const PieceType q[] = {
        PieceType::Straight, PieceType::Straight, PieceType::Straight,
        PieceType::T, PieceType::Cap,
        PieceType::Straight, PieceType::Straight, PieceType::Straight, PieceType::Straight,
    };
    l.queueLength = 9;
    for (int i = 0; i < l.queueLength; ++i) l.queue[i] = q[i];
    return l;
}

} // namespace

const Level &levelConnect() { static const Level level = makeConnect(); return level; }
const Level &levelDontSpill() { static const Level level = makeDontSpill(); return level; }
const Level &levelJunction() { static const Level level = makeJunction(); return level; }

const Level *levelByIndex(int phase) {
    switch (phase) {
        case 0: return &levelConnect();
        case 1: return &levelDontSpill();
        case 2: return &levelJunction();
        default: return nullptr;
    }
}

} // namespace wp
