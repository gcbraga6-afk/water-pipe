cmake_minimum_required(VERSION 3.16)
project(water_pipe_native CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Native build for desktop testing only. The real target is the Frida
# firmware (ESP32-S3 / PlatformIO); see docs/HARDWARE.md and
# docs/TECHNICAL_DESIGN.md. src/core/* here are host-API stubs, not the
# firmware's real drivers.

set(WP_CORE_SOURCES
    src/core/GfxCompat.cpp
    src/core/TouchDriver.cpp
    src/core/audio.cpp
)

set(WP_GAME_SOURCES
    src/games/water_pipe/wp_pieces.cpp
    src/games/water_pipe/wp_board.cpp
    src/games/water_pipe/wp_levels.cpp
    src/games/water_pipe/wp_simulation.cpp
    src/games/water_pipe/wp_inventory.cpp
    src/games/water_pipe/wp_input.cpp
    src/games/water_pipe/wp_render.cpp
    src/games/water_pipe/wp_menu.cpp
    src/games/water_pipe/wp_progress.cpp
    src/games/water_pipe/wp_time.cpp
    src/games/water_pipe/water_pipe.cpp
)

add_library(water_pipe_lib STATIC ${WP_CORE_SOURCES} ${WP_GAME_SOURCES})
target_include_directories(water_pipe_lib PUBLIC src)

enable_testing()

add_executable(wp_tests tests/wp_tests.cpp)
target_link_libraries(wp_tests PRIVATE water_pipe_lib)
add_test(NAME wp_tests COMMAND wp_tests)

add_executable(wp_demo tools/wp_demo.cpp)
target_link_libraries(wp_demo PRIVATE water_pipe_lib)
