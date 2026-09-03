#include "wp_render.h"

#include <algorithm>

#include "core/GfxCompat.h"
#include "wp_layout.h"
#include "wp_pieces.h"

namespace wp {

namespace {

uint16_t col(uint8_t r, uint8_t g, uint8_t b) { return GfxCompat::color565(r, g, b); }

const uint16_t kBoardBgA = col(236, 234, 229);
const uint16_t kBoardBgB = col(226, 223, 216);
const uint16_t kGrid = col(135, 133, 128);
const uint16_t kPipeOutline = col(52, 55, 58);
// Darkened from col(188,194,196) — the old value blended into the board
// background (col(236,234,229)/col(226,223,216)) on the physical panel.
const uint16_t kPvc = col(150, 156, 158);
const uint16_t kPvcHighlight = col(190, 196, 198);
const uint16_t kCopper = col(190, 105, 48);
const uint16_t kCopperHighlight = col(224, 145, 76);
// Water is always this blue — everywhere, including the leak drips below.
// No separate "alert" hue for water: blue means water, full stop, whether
// it's sitting still, flowing, or escaping through an open port.
const uint16_t kWater = col(25, 145, 220);
const uint16_t kWaterHighlight = col(90, 205, 245);
const uint16_t kSource = col(20, 135, 135);
const uint16_t kTarget = col(205, 155, 18);
const uint16_t kHudPanel = col(20, 22, 24);
const uint16_t kHudSlot = col(35, 38, 41);
const uint16_t kHudBorder = col(88, 94, 98);
const uint16_t kHudBorderActive = col(205, 160, 35);
const uint16_t kMeterTrack = col(48, 51, 54);
const uint16_t kMeterLoss = col(210, 70, 40);
// Brighter than before (was col(12,35,64), too close to kHudPanel's
// col(20,22,24) once quantized to RGB565 — the digit tiles were reading as
// hollow outlines instead of distinct boxes on the real panel).
const uint16_t kLiterDigitBg = col(18, 58, 108);
const uint16_t kRestartBtn = col(62, 68, 76);
const uint16_t kExitBtn = col(45, 58, 68);
const uint16_t kVictory = col(45, 180, 85);
const uint16_t kDefeat = col(205, 50, 50);
const uint16_t kWhite = col(245, 245, 245);

uint16_t materialColor(Material m) { return m == Material::Copper ? kCopper : kPvc; }
uint16_t materialHighlight(Material m) { return m == Material::Copper ? kCopperHighlight : kPvcHighlight; }

void drawPipeLayer(GfxCompat *g, int x, int y, int size, uint8_t mask,
                   uint16_t body, uint16_t highlight, int thickness) {
    const int cx = x + size / 2;
    const int cy = y + size / 2;
    const int t = thickness;
    const int radius = std::max(2, t / 4);

    g->fillRoundRect(cx - t / 2, cy - t / 2, t, t, radius, body);
    if (mask & DIR_UP) g->fillRoundRect(cx - t / 2, y + 2, t, size / 2 + t / 2, radius, body);
    if (mask & DIR_DOWN) g->fillRoundRect(cx - t / 2, cy - 2, t, size / 2 + t / 2, radius, body);
    if (mask & DIR_LEFT) g->fillRoundRect(x + 2, cy - t / 2, size / 2 + t / 2, t, radius, body);
    if (mask & DIR_RIGHT) g->fillRoundRect(cx - 2, cy - t / 2, size / 2 + t / 2, t, radius, body);

    const int h = std::max(2, t / 7);
    g->fillRoundRect(cx - t / 2 + 3, cy - t / 2 + 2, t - 6, h, 2, highlight);
}

// The pipe wall (material) and its contents (water) are two independent
// layers now, not one color that swaps depending on state (docs/PIECES.md
// section 6: material is a property of the pipe, not of what's flowing
// through it). A dry copper pipe shows its orange wall; the same pipe full
// of water shows the orange wall with a blue core running through it.
// Thickness floors scale down for small sizes now — this same function
// draws full 64px board cells AND small HUD preview icons (42px for the
// current piece, 22px for upcoming ones). The floors used to be tuned only
// for the board (20/15), so every preview icon hit the same floor
// regardless of its own size and all looked about the same size — the
// smaller "next" queue icons weren't actually reading as smaller than the
// current piece. At board scale (size=64) these floors are never hit
// anyway (21 and 16 are already above them), so board pipes look
// unchanged; only the small previews get thinner.
void drawPipeVisual(GfxCompat *g, int x, int y, int size, uint8_t mask,
                    Material material, bool hasWater) {
    drawPipeLayer(g, x, y, size, mask, kPipeOutline, kPipeOutline, std::max(4, size / 3));

    const int ringThickness = std::max(3, size / 4);
    drawPipeLayer(g, x, y, size, mask, materialColor(material),
                 materialHighlight(material), ringThickness);

    if (hasWater) {
        const int coreThickness = std::max(2, size / 7);
        drawPipeLayer(g, x, y, size, mask, kWater, kWaterHighlight, coreThickness);
    }
}

// Which mask bits have no valid neighbor connection — i.e. where this piece
// is physically open to the outside. Pure/geometric: reads the board only,
// mirrors the check in wp_simulation.cpp's leak detection but has no side
// effects, so it's safe to call from drawCell every frame.
uint8_t openPortMask(Board &board, int col_, int row) {
    const uint8_t mask = board.connectionMaskAt(Board::index(col_, row));
    uint8_t open = 0;
    static constexpr uint8_t kBits[4] = {DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT};
    for (uint8_t bit : kBits) {
        if (!(mask & bit)) continue;
        int dx, dy;
        dirDelta(bit, dx, dy);
        const int nc = col_ + dx, nr = row + dy;
        if (!Board::inBounds(nc, nr)) { open |= bit; continue; }
        const int nIdx = Board::index(nc, nr);
        if (board.at(nIdx).type == PieceType::Empty) { open |= bit; continue; }
        if (!(board.connectionMaskAt(nIdx) & oppositeDir(bit))) open |= bit;
    }
    return open;
}

// Drips spawn at the cell's actual open edge(s) instead of a fixed corner
// dot, so the alert points at *where* the leak is, not just *that* one
// exists somewhere in the cell.
// Drips spawn at the cell's actual open edge(s) instead of a fixed corner
// dot, so the alert points at *where* the leak is, not just *that* one
// exists somewhere in the cell. Several small drops (a splash), not one —
// a single dot reads as a static marker, a spray reads as "something is
// escaping". Always kWater: this is still water, just water that's
// getting away.
void drawSplash(GfxCompat *g, int originX, int originY, int dx, int dy, int px, int py) {
    g->fillCircle(originX + dx * 2 - px * 5, originY + dy * 2 - py * 5, 2, kWater);
    g->fillCircle(originX + dx * 5, originY + dy * 5, 3, kWater);
    g->fillCircle(originX + dx * 2 + px * 5, originY + dy * 2 + py * 5, 2, kWater);
}

void drawLeakDrips(GfxCompat *g, const Rect &r, uint8_t openMask) {
    const int cx = r.x + r.w / 2, cy = r.y + r.h / 2;
    if (openMask & DIR_UP)    drawSplash(g, cx, r.y, 0, -1, 1, 0);
    if (openMask & DIR_DOWN)  drawSplash(g, cx, r.y + r.h, 0, 1, 1, 0);
    if (openMask & DIR_LEFT)  drawSplash(g, r.x, cy, -1, 0, 0, 1);
    if (openMask & DIR_RIGHT) drawSplash(g, r.x + r.w, cy, 1, 0, 0, 1);
}

void drawCell(GfxCompat *g, Board &board, int col_, int row, const SimState &sim) {
    Cell &c = board.at(col_, row);
    if (!c.dirty) return;

    const Rect r = cellRect(col_, row);
    const bool checker = ((col_ + row) & 1) != 0;
    g->fillRect(r.x, r.y, r.w, r.h, checker ? kBoardBgB : kBoardBgA);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 3, kGrid);

    const int idx = Board::index(col_, row);
    const bool leaking = sim.leaking[idx];

    if (c.type == PieceType::Source) {
        drawPipeVisual(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation), Material::PVC, c.volume > 0);
        g->fillCircle(r.x + r.w / 2, r.y + r.h / 2, 10, kSource);
    } else if (c.type == PieceType::Target) {
        drawPipeVisual(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation), Material::PVC, c.volume > 0);
        const int pad = r.w / 4;
        g->drawRoundRect(r.x + pad, r.y + pad, r.w - 2 * pad, r.h - 2 * pad, 5, kTarget);
        const int fillH = (c.volume * (r.h - 2 * pad - 6)) / PIPE_CAPACITY;
        if (fillH > 0) {
            g->fillRect(r.x + pad + 3, r.y + r.h - pad - 3 - fillH,
                        r.w - 2 * pad - 6, fillH, kWater);
        }
    } else if (c.type != PieceType::Empty) {
        drawPipeVisual(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation),
                       c.material, c.volume > 0);
    }

    if (leaking) drawLeakDrips(g, r, openPortMask(board, col_, row));
    c.dirty = false;
}

void drawPreviewSlot(GfxCompat *g, const Rect &r, PieceType type, Material material, uint16_t border) {
    g->fillRoundRect(r.x, r.y, r.w, r.h, 6, kHudSlot);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 6, border);
    if (type == PieceType::Empty) return;

    const int size = std::min(r.w, r.h) - 14;
    const int x = r.x + (r.w - size) / 2;
    const int y = r.y + (r.h - size) / 2;
    drawPipeVisual(g, x, y, size, baseMask(type), material, false);
}

void drawMeter(GfxCompat *g, const Rect &r, int value, int maxValue, uint16_t fillColor) {
    g->fillRoundRect(r.x, r.y, r.w, r.h, 4, kMeterTrack);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 4, kHudBorder);
    if (maxValue <= 0) return;
    const int w = (std::min(value, maxValue) * (r.w - 4)) / maxValue;
    if (w > 0) g->fillRoundRect(r.x + 2, r.y + 2, w, r.h - 4, 3, fillColor);
}

// --- Delivered-water cascade -------------------------------------------
// Each segment darkens from empty to full as its own share of the
// objective fills, then the next segment starts — like pouring water
// into a row of connected vessels. All math is integer permille (0..1000)
// per docs/WATER_SYSTEM.md section 2 ("avoid floating-point-heavy
// simulation where integer/fixed-point arithmetic is sufficient").
struct ColorStop { int permille; uint8_t r, g, b; };
constexpr ColorStop kCascadeStops[4] = {
    {0,    48,  51,  54},   // empty: same as kMeterTrack
    {350, 133, 180, 235},   // starting to fill
    {650,  25, 145, 220},   // same hue as kWater
    {1000, 12,  35,  64},   // full: dark navy
};

int clampInt(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

uint16_t lerpStopColor(int permille) {
    permille = clampInt(permille, 0, 1000);
    for (int i = 0; i < 3; ++i) {
        const ColorStop &a = kCascadeStops[i];
        const ColorStop &b = kCascadeStops[i + 1];
        if (permille <= b.permille) {
            const int span = std::max(1, b.permille - a.permille);
            const int local = permille - a.permille;
            const uint8_t r = a.r + (b.r - a.r) * local / span;
            const uint8_t gc = a.g + (b.g - a.g) * local / span;
            const uint8_t bl = a.b + (b.b - a.b) * local / span;
            return col(r, gc, bl);
        }
    }
    const ColorStop &last = kCascadeStops[3];
    return col(last.r, last.g, last.b);
}

void drawCascade(GfxCompat *g, int delivered, int required, WpHudCache &cache) {
    const int overall = required > 0 ? clampInt(delivered * 1000 / required, 0, 1000) : 0;
    const int perSeg = 1000 / CASCADE_SEGMENTS;
    for (int i = 0; i < CASCADE_SEGMENTS; ++i) {
        const int segStart = i * perSeg;
        const int local = clampInt(overall - segStart, 0, perSeg);
        const int frac = perSeg > 0 ? local * 1000 / perSeg : 0;
        if (frac == cache.cascadeFrac[i]) continue;
        const Rect r = cascadeSegmentRect(i);
        g->fillRoundRect(r.x, r.y, r.w, r.h, 3, lerpStopColor(frac));
        cache.cascadeFrac[i] = frac;
    }
}

// --- Liter counter, 7-segment digits ------------------------------------
// print()/printf() are documented no-ops on the real panel
// (docs/HARDWARE.md section 5), so digits are hand-drawn as bars via
// fillRoundRect — no font system needed for just 0-9.
// Segment bits: A=top B=top-right C=bottom-right D=bottom
//               E=bottom-left F=top-left G=middle
constexpr uint8_t kDigitSegments[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
};

void drawDigit(GfxCompat *g, const Rect &r, int digit, uint16_t bg, uint16_t border, uint16_t fg) {
    g->fillRoundRect(r.x, r.y, r.w, r.h, 3, bg);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 3, border);
    if (digit < 0 || digit > 9) return;
    const uint8_t seg = kDigitSegments[digit];
    const int m = std::max(2, r.w / 8);
    const int st = std::max(3, r.w / 6);
    const int innerW = r.w - 2 * m;
    const int topY = r.y + m;
    const int botY = r.y + r.h - m - st;
    const int midY = r.y + (r.h - st) / 2;
    const int leftX = r.x + m;
    const int rightX = r.x + r.w - m - st;
    const int vTopH = std::max(2, midY - (topY + st));
    const int vBotH = std::max(2, botY - (midY + st));

    if (seg & 0x01) g->fillRoundRect(leftX, topY, innerW, st, 1, fg);
    if (seg & 0x02) g->fillRoundRect(rightX, topY + st, st, vTopH, 1, fg);
    if (seg & 0x04) g->fillRoundRect(rightX, midY + st, st, vBotH, 1, fg);
    if (seg & 0x08) g->fillRoundRect(leftX, botY, innerW, st, 1, fg);
    if (seg & 0x10) g->fillRoundRect(leftX, midY + st, st, vBotH, 1, fg);
    if (seg & 0x20) g->fillRoundRect(leftX, topY + st, st, vTopH, 1, fg);
    if (seg & 0x40) g->fillRoundRect(leftX, midY, innerW, st, 1, fg);
}

// NOTE: sim.delivered is the abstract volume unit from
// docs/WATER_SYSTEM.md section 2 (PIPE_CAPACITY = 10 per segment), not a
// real-world liter conversion the docs ever defined. This counter labels
// that unit as liters for HUD readability; revisit if a real conversion
// factor gets specified later.
void drawLiterCounter(GfxCompat *g, int liters, WpHudCache &cache) {
    liters = clampInt(liters, 0, 9999);
    int digits[LITER_DIGITS];
    int v = liters;
    for (int i = LITER_DIGITS - 1; i >= 0; --i) { digits[i] = v % 10; v /= 10; }
    for (int i = 0; i < LITER_DIGITS; ++i) {
        if (digits[i] == cache.literDigit[i]) continue;
        drawDigit(g, literDigitRect(i), digits[i], kLiterDigitBg, kHudBorder, kWhite);
        cache.literDigit[i] = digits[i];
    }
}

void drawRestartIcon(GfxCompat *g, const Rect &r, uint16_t color) {
    const int cx = r.x + r.w / 2;
    const int cy = r.y + r.h / 2;
    const int rad = 15;
    g->drawRoundRect(cx - rad, cy - rad, rad * 2, rad * 2, rad, color);
    g->fillRect(cx + 3, cy - rad - 2, rad + 7, 12, kRestartBtn);
    g->fillTriangle(cx + rad, cy - rad, cx + rad + 8, cy - rad - 7,
                    cx + rad + 8, cy - rad + 5, color);
}

void drawExitIcon(GfxCompat *g, const Rect &r, uint16_t color) {
    const int x = r.x + 12;
    const int y = r.y + 12;
    g->drawRoundRect(x, y, 22, 32, 3, color);
    g->fillRect(x + 5, y + 5, 17, 22, kExitBtn);
    g->drawLine(x + 8, y + 16, r.x + r.w - 10, y + 16, color);
    g->fillTriangle(r.x + r.w - 10, y + 16, r.x + r.w - 19, y + 9,
                    r.x + r.w - 19, y + 23, color);
}

// A small static downward arrow showing "this slot relates to the one
// above it" — used between the current piece and HOLD, and between the
// two upcoming pieces. Purely decorative and never changes, so it's drawn
// once from renderInit rather than every renderHud call.
void drawArrowDown(GfxCompat *g, const Rect &r, uint16_t color) {
    const int cx = r.x + r.w / 2;
    const int cy = r.y + r.h / 2;
    g->fillTriangle(cx - 5, cy - 3, cx + 5, cy - 3, cx, cy + 4, color);
}

}  // namespace

void renderInit(GfxCompat *g, Board &board) {
    g->fillRect(0, 0, SCREEN_W, SCREEN_H, kHudPanel);
    g->fillRoundRect(BOARD_ORIGIN_X - 4, BOARD_ORIGIN_Y - 4,
                     BOARD_PIXEL_W + 8, BOARD_PIXEL_H + 8, 6, kBoardBgA);
    g->fillRect(HUD_X - 8, 0, SCREEN_W - (HUD_X - 8), SCREEN_H, kHudPanel);
    drawArrowDown(g, HOLD_ARROW_RECT, kHudBorder);
    drawArrowDown(g, QUEUE_ARROW_RECT, kHudBorder);
    board.markAllDirty();
}

void renderBoard(GfxCompat *g, Board &board, const SimState &sim) {
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int c = 0; c < BOARD_COLS; ++c) drawCell(g, board, c, row, sim);
    }
}

void renderHud(GfxCompat *g, const WpInventory &inv, const SimState &sim, const Level &level,
               Outcome outcome, WpHudCache &cache) {
    if (inv.hand() != cache.hand || inv.selectedMaterial() != cache.material) {
        drawPreviewSlot(g, CURRENT_RECT, inv.hand(), inv.selectedMaterial(), kHudBorderActive);
        cache.hand = inv.hand();
        cache.material = inv.selectedMaterial();
    }
    if (inv.peekUpcoming(0) != cache.upcoming0) {
        drawPreviewSlot(g, QUEUE_RECT[0], inv.peekUpcoming(0), Material::PVC, kHudBorder);
        cache.upcoming0 = inv.peekUpcoming(0);
    }
    if (inv.peekUpcoming(1) != cache.upcoming1) {
        drawPreviewSlot(g, QUEUE_RECT[1], inv.peekUpcoming(1), Material::PVC, kHudBorder);
        cache.upcoming1 = inv.peekUpcoming(1);
    }
    if (inv.held() != cache.held || inv.hasHeld() != cache.hasHeld) {
        // NOTE: WpInventory doesn't remember which material was selected at
        // the moment a piece was held (Snapshot.held is a bare PieceType),
        // so the held preview always shows the *current* toggle state, not
        // whatever it was when the piece went into hold. Fine for now since
        // material has no gameplay effect yet; revisit if that changes.
        drawPreviewSlot(g, HOLD_RECT, inv.hasHeld() ? inv.held() : PieceType::Empty,
                        inv.selectedMaterial(), inv.hasHeld() ? kHudBorderActive : kHudBorder);
        cache.held = inv.held();
        cache.hasHeld = inv.hasHeld();
    }

    if (inv.selectedMaterial() != cache.materialButtons) {
        const bool isPvc = inv.selectedMaterial() == Material::PVC;
        g->fillRoundRect(MATERIAL_PVC_RECT.x, MATERIAL_PVC_RECT.y, MATERIAL_PVC_RECT.w, MATERIAL_PVC_RECT.h, 5, kPvc);
        g->drawRoundRect(MATERIAL_PVC_RECT.x, MATERIAL_PVC_RECT.y, MATERIAL_PVC_RECT.w, MATERIAL_PVC_RECT.h, 5,
                         isPvc ? kHudBorderActive : kHudBorder);
        g->fillRoundRect(MATERIAL_CU_RECT.x, MATERIAL_CU_RECT.y, MATERIAL_CU_RECT.w, MATERIAL_CU_RECT.h, 5, kCopper);
        g->drawRoundRect(MATERIAL_CU_RECT.x, MATERIAL_CU_RECT.y, MATERIAL_CU_RECT.w, MATERIAL_CU_RECT.h, 5,
                         isPvc ? kHudBorder : kHudBorderActive);
        cache.materialButtons = inv.selectedMaterial();
    }

    const int lossBucket = sim.totalLoss / 4;
    if (lossBucket != cache.lossBucket) {
        drawMeter(g, LOSS_METER_RECT, sim.totalLoss, std::max(1, level.requiredVolume), kMeterLoss);
        cache.lossBucket = lossBucket;
    }
    drawCascade(g, sim.delivered, level.requiredVolume, cache);
    drawLiterCounter(g, sim.delivered, cache);

    if (outcome != cache.outcome) {
        const uint16_t frame = outcome == Outcome::Victory ? kVictory
                             : outcome == Outcome::Defeat ? kDefeat : kGrid;
        for (int i = 0; i < 3; ++i) {
            g->drawRoundRect(BOARD_ORIGIN_X - 1 - i, BOARD_ORIGIN_Y - 1 - i,
                             BOARD_PIXEL_W + 2 + 2 * i, BOARD_PIXEL_H + 2 + 2 * i, 5, frame);
        }
        cache.outcome = outcome;
    }

    // These controls stay visible throughout the phase, including after a
    // victory/defeat. Restart is always the current phase; exit saves and
    // returns to the Water Pipe main menu.
    g->fillRoundRect(RESTART_RECT.x, RESTART_RECT.y, RESTART_RECT.w, RESTART_RECT.h, 6,
                     outcome == Outcome::Defeat ? kDefeat : kRestartBtn);
    g->drawRoundRect(RESTART_RECT.x, RESTART_RECT.y, RESTART_RECT.w, RESTART_RECT.h, 6, kHudBorder);
    drawRestartIcon(g, RESTART_RECT, kWhite);

    g->fillRoundRect(EXIT_RECT.x, EXIT_RECT.y, EXIT_RECT.w, EXIT_RECT.h, 6, kExitBtn);
    g->drawRoundRect(EXIT_RECT.x, EXIT_RECT.y, EXIT_RECT.w, EXIT_RECT.h, 6, kHudBorder);
    drawExitIcon(g, EXIT_RECT, kWhite);
}

}  // namespace wp
