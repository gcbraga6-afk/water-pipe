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
const uint16_t kPvc = col(188, 194, 196);
const uint16_t kPvcHighlight = col(226, 231, 232);
const uint16_t kCopper = col(190, 105, 48);
const uint16_t kCopperHighlight = col(224, 145, 76);
const uint16_t kWater = col(25, 145, 220);
const uint16_t kWaterHighlight = col(90, 205, 245);
const uint16_t kWaterLeak = col(220, 65, 35);
const uint16_t kLeakDot = col(255, 45, 20);
const uint16_t kSource = col(20, 135, 135);
const uint16_t kTarget = col(205, 155, 18);
const uint16_t kHudPanel = col(20, 22, 24);
const uint16_t kHudSlot = col(35, 38, 41);
const uint16_t kHudBorder = col(88, 94, 98);
const uint16_t kHudBorderActive = col(205, 160, 35);
const uint16_t kMeterTrack = col(48, 51, 54);
const uint16_t kMeterLoss = col(210, 70, 40);
const uint16_t kMeterDelivered = col(45, 165, 90);
const uint16_t kRestartBtn = col(62, 68, 76);
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

    // Small highlight on the upper/left side makes the pipe readable against
    // the light tile even when it is empty.
    const int h = std::max(2, t / 7);
    g->fillRoundRect(cx - t / 2 + 3, cy - t / 2 + 2, t - 6, h, 2, highlight);
}

void drawPipeVisual(GfxCompat *g, int x, int y, int size, uint8_t mask,
                    Material material, bool water, bool leaking) {
    // Dark silhouette first: this is the key contrast against the pale tile.
    drawPipeLayer(g, x, y, size, mask, kPipeOutline, kPipeOutline, std::max(20, size / 3));

    const uint16_t body = water ? (leaking ? kWaterLeak : kWater) : materialColor(material);
    const uint16_t highlight = water ? kWaterHighlight : materialHighlight(material);
    drawPipeLayer(g, x, y, size, mask, body, highlight, std::max(13, size / 5));
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
        drawPipeVisual(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation), Material::PVC, c.volume > 0, leaking);
        g->fillCircle(r.x + r.w / 2, r.y + r.h / 2, 10, kSource);
    } else if (c.type == PieceType::Target) {
        drawPipeVisual(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation), Material::PVC, c.volume > 0, false);
        const int pad = r.w / 4;
        g->drawRoundRect(r.x + pad, r.y + pad, r.w - 2 * pad, r.h - 2 * pad, 5, kTarget);
        const int fillH = (c.volume * (r.h - 2 * pad - 6)) / PIPE_CAPACITY;
        if (fillH > 0) {
            g->fillRect(r.x + pad + 3, r.y + r.h - pad - 3 - fillH,
                        r.w - 2 * pad - 6, fillH, kWater);
        }
    } else if (c.type != PieceType::Empty) {
        drawPipeVisual(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation),
                       c.material, c.volume > 0, leaking);
    }

    if (leaking) {
        g->fillCircle(r.x + r.w - 9, r.y + 9, 6, kLeakDot);
    }

    c.dirty = false;
}

void drawPreviewSlot(GfxCompat *g, const Rect &r, PieceType type, Material material, uint16_t border) {
    g->fillRoundRect(r.x, r.y, r.w, r.h, 6, kHudSlot);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 6, border);
    if (type == PieceType::Empty) return;

    const int size = std::min(r.w, r.h) - 14;
    const int x = r.x + (r.w - size) / 2;
    const int y = r.y + (r.h - size) / 2;
    drawPipeVisual(g, x, y, size, baseMask(type), material, false, false);
}

void drawMeter(GfxCompat *g, const Rect &r, int value, int maxValue, uint16_t fillColor) {
    g->fillRoundRect(r.x, r.y, r.w, r.h, 4, kMeterTrack);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 4, kHudBorder);
    if (maxValue <= 0) return;
    const int w = (std::min(value, maxValue) * (r.w - 4)) / maxValue;
    if (w > 0) g->fillRoundRect(r.x + 2, r.y + 2, w, r.h - 4, 3, fillColor);
}

}  // namespace

void renderInit(GfxCompat *g, Board &board) {
    // Dark surround first. The board is deliberately the only large light
    // surface, eliminating the accidental white strip below the old HUD.
    g->fillRect(0, 0, SCREEN_W, SCREEN_H, kHudPanel);
    g->fillRoundRect(BOARD_ORIGIN_X - 4, BOARD_ORIGIN_Y - 4,
                     BOARD_PIXEL_W + 8, BOARD_PIXEL_H + 8, 6, kBoardBgA);
    g->fillRect(HUD_X - 8, 0, SCREEN_W - (HUD_X - 8), SCREEN_H, kHudPanel);
    board.markAllDirty();
}

void renderBoard(GfxCompat *g, Board &board, const SimState &sim) {
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int c = 0; c < BOARD_COLS; ++c) drawCell(g, board, c, row, sim);
    }
}

void renderHud(GfxCompat *g, const WpInventory &inv, const SimState &sim, const Level &level, Outcome outcome,
               bool /*removeMode*/, WpHudCache &cache) {
    if (inv.hand() != cache.hand) {
        drawPreviewSlot(g, QUEUE_RECT[0], inv.hand(), Material::PVC, kHudBorderActive);
        cache.hand = inv.hand();
    }
    if (inv.peekUpcoming(0) != cache.upcoming0) {
        drawPreviewSlot(g, QUEUE_RECT[1], inv.peekUpcoming(0), Material::PVC, kHudBorder);
        cache.upcoming0 = inv.peekUpcoming(0);
    }
    if (inv.peekUpcoming(1) != cache.upcoming1) {
        drawPreviewSlot(g, QUEUE_RECT[2], inv.peekUpcoming(1), Material::PVC, kHudBorder);
        cache.upcoming1 = inv.peekUpcoming(1);
    }
    if (inv.held() != cache.held || inv.hasHeld() != cache.hasHeld) {
        drawPreviewSlot(g, HOLD_RECT, inv.hasHeld() ? inv.held() : PieceType::Empty,
                        Material::PVC, inv.hasHeld() ? kHudBorderActive : kHudBorder);
        cache.held = inv.held();
        cache.hasHeld = inv.hasHeld();
    }

    const int lossBucket = sim.totalLoss / 4;
    if (lossBucket != cache.lossBucket) {
        drawMeter(g, LOSS_METER_RECT, sim.totalLoss, std::max(1, level.requiredVolume), kMeterLoss);
        cache.lossBucket = lossBucket;
    }
    const int deliveredBucket = sim.delivered / 4;
    if (deliveredBucket != cache.deliveredBucket) {
        drawMeter(g, DELIVERED_METER_RECT, sim.delivered, level.requiredVolume, kMeterDelivered);
        cache.deliveredBucket = deliveredBucket;
    }

    if (outcome != cache.outcome) {
        const uint16_t btnColor = outcome == Outcome::Victory ? kVictory
                                   : outcome == Outcome::Defeat ? kDefeat
                                                                 : kRestartBtn;
        g->fillRoundRect(RESTART_RECT.x, RESTART_RECT.y, RESTART_RECT.w, RESTART_RECT.h, 6, btnColor);
        g->drawRoundRect(RESTART_RECT.x, RESTART_RECT.y, RESTART_RECT.w, RESTART_RECT.h, 6, kHudBorder);

        const int cx = RESTART_RECT.x + RESTART_RECT.w / 2;
        const int cy = RESTART_RECT.y + RESTART_RECT.h / 2;
        const int rad = RESTART_RECT.h / 3;
        g->drawRoundRect(cx - rad, cy - rad, rad * 2, rad * 2, rad, kWhite);
        g->fillTriangle(cx + rad, cy - rad, cx + rad + 6, cy - rad - 6,
                        cx + rad + 6, cy - rad + 6, kWhite);

        const uint16_t frame = outcome == Outcome::Victory ? kVictory : outcome == Outcome::Defeat ? kDefeat : kGrid;
        for (int i = 0; i < 3; ++i) {
            g->drawRoundRect(BOARD_ORIGIN_X - 1 - i, BOARD_ORIGIN_Y - 1 - i,
                             BOARD_PIXEL_W + 2 + 2 * i, BOARD_PIXEL_H + 2 + 2 * i, 5, frame);
        }
        cache.outcome = outcome;
    }
}

}  // namespace wp
