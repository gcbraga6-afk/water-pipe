#include "wp_progress.h"

#include <algorithm>

#if defined(ARDUINO) && __has_include(<Preferences.h>)
#include <Preferences.h>
#define WP_HAS_PREFERENCES 1
#else
#define WP_HAS_PREFERENCES 0
#endif

namespace wp {

namespace {
constexpr uint32_t kMagic = PROGRESS_MAGIC;
constexpr uint16_t kVersion = PROGRESS_VERSION;
constexpr char kNamespace[] = "waterpipe";
constexpr char kKey[] = "save";

#if !WP_HAS_PREFERENCES
ProgressData ramData;
bool ramValid = false;
#endif

bool valid(const ProgressData &data) {
    return data.magic == kMagic && data.version == kVersion &&
           data.unlockedPhase < MAX_PHASES;
}
}  // namespace

ProgressData ProgressStore::load() const {
    ProgressData data{};
#if WP_HAS_PREFERENCES
    Preferences prefs;
    if (!prefs.begin(kNamespace, true)) return data;
    const size_t bytes = prefs.getBytes(kKey, &data, sizeof(data));
    prefs.end();
    if (bytes != sizeof(data) || !valid(data)) return ProgressData{};
#else
    if (!ramValid) return data;
    data = ramData;
    if (!valid(data)) return ProgressData{};
#endif
    return data;
}

bool ProgressStore::save(const ProgressData &data) const {
    if (!valid(data)) return false;
#if WP_HAS_PREFERENCES
    Preferences prefs;
    if (!prefs.begin(kNamespace, false)) return false;
    const size_t bytes = prefs.putBytes(kKey, &data, sizeof(data));
    prefs.end();
    return bytes == sizeof(data);
#else
    ramData = data;
    ramValid = true;
    return true;
#endif
}

bool ProgressStore::clearResume(ProgressData &data) const {
    data.resume = ResumeSnapshot{};
    return save(data);
}

void ProgressStore::recordPhaseResult(ProgressData &data, int phase, int score, int stars) {
    if (phase < 0 || phase >= MAX_PHASES) return;
    if (stars < 0) stars = 0;
    if (stars > 3) stars = 3;
    data.stars[phase] = std::max(data.stars[phase], static_cast<uint8_t>(stars));
    const int nextPhase = phase + 1;
    if (nextPhase < IMPLEMENTED_PHASES)
        data.unlockedPhase = std::max(data.unlockedPhase, static_cast<uint8_t>(nextPhase));

    if (score <= 0) return;
    for (int i = 0; i < MAX_HIGHSCORES; ++i) {
        if (score > data.highScores[i].score) {
            for (int j = MAX_HIGHSCORES - 1; j > i; --j)
                data.highScores[j] = data.highScores[j - 1];
            data.highScores[i] = {score, static_cast<uint8_t>(phase)};
            break;
        }
    }
}

}  // namespace wp
