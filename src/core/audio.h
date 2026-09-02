#pragma once
// Compatibility stub for the Frida firmware's src/core/audio.h /
// audio.cpp (see docs/HARDWARE.md, section 4). Real effects run on a
// dedicated FreeRTOS task on core 0; calling code only ever enqueues and
// returns immediately. Water Pipe follows the exact same pattern: one
// enum case per effect, one enfileirar() call, no blocking i2s_write
// from inside the game loop.
//
// Replace with the real firmware files when integrating into Frida —
// add the WpXxx cases below to the real `enum class Efeito` and give
// each a tocarXxx() implementation there (reusing tocarBeep /
// tocarBeepSweep as the reference notes).

namespace Audio {

enum class Efeito {
    WpPlace,
    WpRotate,
    WpRemove,
    WpHold,
    WpFlowStart,
    WpLeak,
    WpVictory,
    WpDefeat,
};

// Enqueues an effect for the audio task; never blocks.
void enfileirar(Efeito efeito);

// Public, game-facing wrappers — the only API Water Pipe calls directly.
void wpPlace();
void wpRotate();
void wpRemove();
void wpHold();
void wpFlowStart();
void wpLeak();
void wpVictory();
void wpDefeat();

}  // namespace Audio
