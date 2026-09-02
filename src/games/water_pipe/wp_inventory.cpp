#include "wp_inventory.h"

#include <utility>

namespace wp {

void WpInventory::load(const Level &level) {
    length_ = level.queueLength;
    for (int i = 0; i < length_; ++i) queue_[i] = level.queue[i];
    nextIndex_ = 0;
    held_ = PieceType::Empty;
    hasHeld_ = false;
    hand_ = draw();
}

PieceType WpInventory::draw() {
    if (nextIndex_ < length_) return queue_[nextIndex_++];
    return PieceType::Empty;
}

PieceType WpInventory::peekUpcoming(int aheadIndex) const {
    const int idx = nextIndex_ + aheadIndex;
    return idx < length_ ? queue_[idx] : PieceType::Empty;
}

bool WpInventory::empty() const {
    const bool heldUsable = hasHeld_ && held_ != PieceType::Empty;
    return hand_ == PieceType::Empty && !heldUsable;
}

void WpInventory::consumeHand() { hand_ = draw(); }

void WpInventory::swapHold() {
    if (!hasHeld_) {
        held_ = hand_;
        hasHeld_ = true;
        hand_ = draw();
    } else {
        std::swap(hand_, held_);
    }
}

}  // namespace wp
