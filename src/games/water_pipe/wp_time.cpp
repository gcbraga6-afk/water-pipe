#include "wp_time.h"

#if defined(ARDUINO)
#include <Arduino.h>

namespace wp {
uint32_t nowMs() { return millis(); }
}  // namespace wp

#else
#include <chrono>

namespace wp {
uint32_t nowMs() {
    using namespace std::chrono;
    return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}
}  // namespace wp

#endif
