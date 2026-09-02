#pragma once
// Compatibility stub for the Frida firmware's src/core/TouchDriver.h
// (see docs/HARDWARE.md, section 3). Mirrors the real GT911 driver's
// public API — a single cached touch point, no multitouch — so this
// game module compiles unmodified once dropped into the firmware tree.
//
// Replace with the real firmware file when integrating into Frida.

namespace TouchDriver {

bool rawTouched();
int rawX();
int rawY();

// True exactly once per physical tap that lands inside [x0,y0]-[x1,y1].
bool consumeTapInArea(int x0, int y0, int x1, int y1);

// Test-only: simulate a tap at the given point, consumed by the next
// matching consumeTapInArea() call. Not part of the real firmware API.
void simulateTap(int x, int y);
void reset();

}  // namespace TouchDriver
