#include "TouchDriver.h"

namespace TouchDriver {

namespace {
bool pending = false;
int touchX = 0;
int touchY = 0;
}  // namespace

bool rawTouched() { return pending; }
int rawX() { return touchX; }
int rawY() { return touchY; }

bool consumeTapInArea(int x0, int y0, int x1, int y1) {
    if (!pending) return false;
    if (touchX < x0 || touchX > x1 || touchY < y0 || touchY > y1) return false;
    pending = false;
    return true;
}

void simulateTap(int x, int y) {
    touchX = x;
    touchY = y;
    pending = true;
}

void reset() { pending = false; }

}  // namespace TouchDriver
