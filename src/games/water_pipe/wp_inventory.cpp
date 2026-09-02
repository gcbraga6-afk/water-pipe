#include "wp_inventory.h"

#include <utility>

namespace wp {

void WpInventory::load(const Level &level) {
    length_ = level.queueLength;
    for (int i = 0; i < length_; ++i) queue_[i] = level.queue[i];
    nextIndex_ = 0;
    held_ = PieceType::Empty;
    hasHeld_ = false;
    holdUsed_ = false;
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

void WpInventory::consumeHand() {
    hand_ = draw();
    holdUsed_ = false;
}

void WpInventory::swapHold() {
    if (holdUsed_ || hand_ == PieceType::Empty) return;
    if (!hasHeld_) {
        held_ = hand_;
        hasHeld_ = true;
        hand_ = draw();
    } else {
        std::swap(hand_, held_);
    }
    holdUsed_ = true;
}

WpInventory::Snapshot WpInventory::snapshot() const {
    Snapshot s;
    for (int i = 0; i < MAX_QUEUE; ++i) s.queue[i] = queue_[i];
    s.length = length_;
    s.nextIndex = nextIndex_;
    s.hand = hand_;
    s.held = held_;
    s.hasHeld = hasHeld_;
    s.holdUsed = holdUsed_;
    return s;
}

void WpInventory::restore(const Snapshot &s) {
    for (int i = 0; i < MAX_QUEUE; ++i) queue_[i] = s.queue[i];
    length_ = s.length;
    nextIndex_ = s.nextIndex;
    hand_ = s.hand;
    held_ = s.held;
    hasHeld_ = s.hasHeld;
    holdUsed_ = s.holdUsed;
}

}  // namespace wp
