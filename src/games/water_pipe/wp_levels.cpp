#include "wp_levels.h"

namespace wp {

const Level &levelConnect() {
    static const Level level = [] {
        Level l{};
        l.name = "Phase 1 - Connect";

        // Source at the left edge, port facing right into the board.
        l.sourceCol = 0;
        l.sourceRow = 2;
        l.sourceRotation = Rotation::R90;

        // Target near the right edge, one row down, port facing left.
        l.targetCol = 9;
        l.targetRow = 3;
        l.targetRotation = Rotation::R270;

        l.requiredVolume = 60;
        // Give the player a brief construction window before water starts.
        l.sourceDelayMs = 1500;

        const PieceType kQueue[] = {
            PieceType::Straight, PieceType::Straight, PieceType::Straight, PieceType::Straight,
            PieceType::Curve,    PieceType::Curve,
            PieceType::Straight, PieceType::Straight, PieceType::Straight,
            PieceType::T,        PieceType::Cross,    PieceType::Cap,
            PieceType::Straight, PieceType::Curve,
        };
        l.queueLength = static_cast<int>(sizeof(kQueue) / sizeof(kQueue[0]));
        for (int i = 0; i < l.queueLength; ++i) l.queue[i] = kQueue[i];

        return l;
    }();
    return level;
}

}  // namespace wp
