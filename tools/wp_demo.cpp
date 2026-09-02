// Manual smoke test: drives WaterPipe::begin()/loop() through simulated
// taps exactly as the real touch driver would deliver them, building the
// "Phase 1 - Connect" reference solution, then lets the simulation run
// until victory. Confirms the full input -> board -> simulation ->
// renderer path compiles and behaves, and dumps a PPM screenshot of the
// final frame for visual inspection.
//
// Not part of `ctest` (it sleeps in real time to advance the fixed-step
// simulation) — run it manually: ./wp_demo

#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

#include "core/GfxCompat.h"
#include "core/TouchDriver.h"
#include "games/water_pipe/wp_layout.h"
#include "games/water_pipe/water_pipe.h"

namespace {

void tapCellAndSettle(int col, int row) {
    const wp::Rect r = wp::cellRect(col, row);
    TouchDriver::simulateTap(r.x + r.w / 2, r.y + r.h / 2);
    WaterPipe::loop();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

}  // namespace

int main() {
    GfxCompat framebuffer(800, 480);
    gfx = &framebuffer;

    WaterPipe::begin();
    WaterPipe::loop();

    struct Placement {
        int col, row, rotations;
    };
    const std::vector<Placement> path = {
        {1, 2, 1}, {2, 2, 1}, {3, 2, 1}, {4, 2, 1}, {5, 2, 2}, {5, 3, 0}, {6, 3, 1}, {7, 3, 1}, {8, 3, 1},
    };

    for (const Placement &p : path) {
        tapCellAndSettle(p.col, p.row);  // place
        for (int i = 0; i < p.rotations; ++i) tapCellAndSettle(p.col, p.row);  // rotate
    }

    // Let the fixed-step simulation carry water to the target.
    for (int i = 0; i < 400; ++i) {
        WaterPipe::loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    framebuffer.writePPM("wp_demo.ppm");
    std::printf("wp_demo: frame written to wp_demo.ppm\n");
    return 0;
}
