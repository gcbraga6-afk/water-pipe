#include "wp_levels.h"

namespace wp {

const Level &levelConnect() {
    static const Level level = [] {
        Level l{};
        l.name = "Phase 1 - Connect";

        // Source at the left edge, port facing right into the board.
        l.sourceCol = 0;
        l.sourceRow = 2;
        l.sourceRotation = Rotation::R90;  // base UP -> RIGHT

        // Target near the right edge, one row down, port facing left.
        l.targetCol = 9;
        l.targetRow = 3;
        l.targetRotation = Rotation::R270;  // base UP -> LEFT

        l.requiredVolume = 60;
        l.sourceDelayMs = 0;  // starts immediately (docs/GAMEPLAY.md section 15)

        // A reference solution needs 7 straights + 2 curves (4 straights
        // across row 2, a curve down into row 3, a curve back to
        // horizontal, then 3 more straights into the target). The queue
        // provides a little slack plus one each of T/Cross/Cap so the
        // player can discover alternate routes.
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
