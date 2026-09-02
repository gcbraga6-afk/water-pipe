#include "audio.h"

namespace Audio {

void enfileirar(Efeito) {
    // Stub: the real firmware pushes onto the core-0 audio task queue
    // here. Intentionally a no-op in this standalone build.
}

void wpPlace() { enfileirar(Efeito::WpPlace); }
void wpRotate() { enfileirar(Efeito::WpRotate); }
void wpRemove() { enfileirar(Efeito::WpRemove); }
void wpHold() { enfileirar(Efeito::WpHold); }
void wpFlowStart() { enfileirar(Efeito::WpFlowStart); }
void wpLeak() { enfileirar(Efeito::WpLeak); }
void wpVictory() { enfileirar(Efeito::WpVictory); }
void wpDefeat() { enfileirar(Efeito::WpDefeat); }

}  // namespace Audio
