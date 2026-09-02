#include "GfxCompat.h"

#include <algorithm>
#include <cstdio>

GfxCompat *gfx = nullptr;

GfxCompat::GfxCompat(int width, int height)
    : width_(width), height_(height), fb_(new uint16_t[width * height]) {
    fillScreen(0);
}

GfxCompat::~GfxCompat() { delete[] fb_; }

uint16_t GfxCompat::color565(uint8_t r, uint8_t g, uint8_t b) {
    return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void GfxCompat::beginFrame() {}
void GfxCompat::endFrame() {}

void GfxCompat::drawPixel(int x, int y, uint16_t color) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) return;
    fb_[y * width_ + x] = color;
}

void GfxCompat::fillScreen(uint16_t color) {
    fillRect(0, 0, width_, height_, color);
}

void GfxCompat::fillRect(int x, int y, int w, int h, uint16_t color) {
    int x0 = std::max(x, 0);
    int y0 = std::max(y, 0);
    int x1 = std::min(x + w, width_);
    int y1 = std::min(y + h, height_);
    for (int yy = y0; yy < y1; ++yy) {
        for (int xx = x0; xx < x1; ++xx) {
            fb_[yy * width_ + xx] = color;
        }
    }
}

void GfxCompat::fillCircle(int cx, int cy, int r, uint16_t color) {
    for (int y = -r; y <= r; ++y) {
        for (int x = -r; x <= r; ++x) {
            if (x * x + y * y <= r * r) drawPixel(cx + x, cy + y, color);
        }
    }
}

void GfxCompat::drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        drawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void GfxCompat::drawRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
    drawLine(x + r, y, x + w - r, y, color);
    drawLine(x + r, y + h - 1, x + w - r, y + h - 1, color);
    drawLine(x, y + r, x, y + h - r, color);
    drawLine(x + w - 1, y + r, x + w - 1, y + h - r, color);
}

void GfxCompat::fillRoundRect(int x, int y, int w, int h, int r, uint16_t color) {
    fillRect(x + r, y, w - 2 * r, h, color);
    fillRect(x, y + r, w, h - 2 * r, color);
    fillCircle(x + r, y + r, r, color);
    fillCircle(x + w - r - 1, y + r, r, color);
    fillCircle(x + r, y + h - r - 1, r, color);
    fillCircle(x + w - r - 1, y + h - r - 1, r, color);
}

void GfxCompat::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    int minY = std::min({y0, y1, y2});
    int maxY = std::max({y0, y1, y2});
    auto edgeX = [](int ax, int ay, int bx, int by, int y) -> float {
        if (ay == by) return static_cast<float>(ax);
        return ax + (bx - ax) * (static_cast<float>(y - ay) / (by - ay));
    };
    for (int y = minY; y <= maxY; ++y) {
        float xs[3];
        int n = 0;
        if ((y0 <= y && y <= y1) || (y1 <= y && y <= y0)) xs[n++] = edgeX(x0, y0, x1, y1, y);
        if ((y1 <= y && y <= y2) || (y2 <= y && y <= y1)) xs[n++] = edgeX(x1, y1, x2, y2, y);
        if ((y2 <= y && y <= y0) || (y0 <= y && y <= y2)) xs[n++] = edgeX(x2, y2, x0, y0, y);
        if (n < 2) continue;
        float lo = std::min(xs[0], xs[1]);
        float hi = std::max(xs[0], xs[1]);
        for (int k = 2; k < n; ++k) {
            lo = std::min(lo, xs[k]);
            hi = std::max(hi, xs[k]);
        }
        fillRect(static_cast<int>(lo), y, static_cast<int>(hi - lo) + 1, 1, color);
    }
}

void GfxCompat::print(const char *) {
    // No-op: matches real hardware today (docs/HARDWARE.md, section 5).
}

void GfxCompat::printf(const char *, ...) {
    // No-op: matches real hardware today (docs/HARDWARE.md, section 5).
}

void GfxCompat::writePPM(const char *path) const {
    FILE *f = std::fopen(path, "wb");
    if (!f) return;
    std::fprintf(f, "P6\n%d %d\n255\n", width_, height_);
    for (int i = 0; i < width_ * height_; ++i) {
        uint16_t px = fb_[i];
        uint8_t r = static_cast<uint8_t>((px >> 8) & 0xF8);
        uint8_t g = static_cast<uint8_t>((px >> 3) & 0xFC);
        uint8_t b = static_cast<uint8_t>((px << 3) & 0xF8);
        std::fputc(r, f);
        std::fputc(g, f);
        std::fputc(b, f);
    }
    std::fclose(f);
}
