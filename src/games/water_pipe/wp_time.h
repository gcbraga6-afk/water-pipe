#pragma once
// Wall-clock time source. On the real firmware this is Arduino's
// millis(); natively (tests, this standalone build) it falls back to
// std::chrono so the simulation's fixed timestep still advances
// correctly outside the ESP32 toolchain.

#include <cstdint>

namespace wp {

uint32_t nowMs();

}  // namespace wp
