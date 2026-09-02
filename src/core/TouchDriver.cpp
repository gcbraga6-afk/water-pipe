#include "TouchDriver.h"

namespace TouchDriver {

namespace {
bool pending = false;
bool simulated = false;
int touchX = 0;
int touchY = 0;
}  // namespace

bool rawTouched() { return pending && !simulated; }
int rawX() { return touchX; }
int rawY() { return touchY; }

bool consumeTapInArea(int x0, int y0, int x1, int y1) {
    if (!pending) return false;
    if (touchX < x0 || touchX > x1 || touchY < y0 || touchY > y1) return false;
    pending = false;
    simulated = false;
    return true;
}

void simulateTap(int x, int y) {
    touchX = x;
    touchY = y;
    simulated = true;
    pending = true;
}

void reset() {
    pending = false;
    simulated = false;
}

}  // namespace TouchDriver
