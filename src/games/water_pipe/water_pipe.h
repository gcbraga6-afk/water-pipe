#pragma once
// Water Pipe entry point. Same contract as Artilharia
// (docs/HARDWARE.md section 6): begin() once when the tile is tapped in
// the Games menu, loop() every frame while active. loop() returns true
// when the player wants to exit back to the Games menu.

namespace WaterPipe {

void begin();
bool loop();

}  // namespace WaterPipe
