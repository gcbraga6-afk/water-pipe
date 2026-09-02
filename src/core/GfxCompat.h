#pragma once
// Compatibility stub for the Frida firmware's src/core/GfxCompat.h
// (see docs/HARDWARE.md, section 2). Mirrors the real host API exactly so
// this game module compiles unmodified once dropped into the firmware
// tree. Backed here by a plain in-memory RGB565 framebuffer so the game
// logic can be built and exercised natively (see tests/).
//
// Replace this header (and GfxCompat.cpp) with the real firmware files
// when integrating Water Pipe into Frida — do not ship this stub on
// device.

#include <cstdint>
#include <cstdarg>
#include <cstddef>

class GfxCompat {
public:
    GfxCompat(int width, int height);
    ~GfxCompat();

    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

    int width() const { return width_; }
    int height() const { return height_; }

    void beginFrame();
    void endFrame();

    void fillScreen(uint16_t color);
    void drawPixel(int x, int y, uint16_t color);
    void fillRect(int x, int y, int w, int h, uint16_t color);
    void fillCircle(int cx, int cy, int r, uint16_t color);
    void drawLine(int x0, int y0, int x1, int y1, uint16_t color);
    void drawRoundRect(int x, int y, int w, int h, int r, uint16_t color);
    void fillRoundRect(int x, int y, int w, int h, int r, uint16_t color);
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);

    // Text rendering is a documented no-op on real hardware today
    // (docs/HARDWARE.md, section 5). Water Pipe must not depend on it.
    void print(const char *text);
    void printf(const char *fmt, ...);

    // Test-only helpers, not part of the real firmware API.
    const uint16_t *framebuffer() const { return fb_; }
    void writePPM(const char *path) const;

private:
    int width_;
    int height_;
    uint16_t *fb_;
};

extern GfxCompat *gfx;
