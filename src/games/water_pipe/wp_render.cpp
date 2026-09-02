#include "wp_render.h"

#include <algorithm>

#include "core/GfxCompat.h"
#include "wp_layout.h"
#include "wp_pieces.h"

namespace wp {

namespace {

uint16_t col(uint8_t r, uint8_t g, uint8_t b) { return GfxCompat::color565(r, g, b); }

// Board tiles stay light for readability, while everything outside the board
// is deliberately dark. This removes the old white strip below the grid.
uint16_t kBoardBgA = col(218, 222, 224);
uint16_t kBoardBgB = col(204, 210, 213);
uint16_t kGrid = col(105, 113, 118);
uint16_t kBoardFrame = col(55, 62, 66);

// Pipes use a dark outer silhouette plus a brighter material body. This is
// much easier to read against the pale tiles than the previous near-white PVC.
uint16_t kPipeShadow = col(34, 40, 44);
uint16_t kPvc = col(92, 112, 124);
uint16_t kPvcHighlight = col(170, 190, 198);
uint16_t kCopper = col(181, 91, 35);
uint16_t kCopperHighlight = col(235, 157, 82);

uint16_t kWater = col(25, 135, 225);
uint16_t kWaterHighlight = col(100, 205, 255);
uint16_t kWaterLeak = col(220, 70, 30);
uint16_t kLeakDot = col(255, 40, 20);
uint16_t kSource = col(12, 128, 132);
uint16_t kTarget = col(205, 155, 12);
uint16_t kHudPanel = col(20, 23, 25);
uint16_t kHudSlot = col(34, 39, 42);
uint16_t kHudBorder = col(78, 88, 94);
uint16_t kHudBorderActive = col(220, 170, 45);
uint16_t kMeterTrack = col(48, 54, 58);
uint16_t kMeterLoss = col(210, 70, 40);
uint16_t kMeterDelivered = col(35, 160, 215);
uint16_t kRestartBtn = col(58, 72, 92);
uint16_t kVictory = col(40, 175, 90);
uint16_t kDefeat = col(210, 50, 50);
uint16_t kWhite = col(245, 245, 245);

uint16_t materialColor(Material m) { return m == Material::Copper ? kCopper : kPvc; }
uint16_t materialHighlight(Material m) { return m == Material::Copper ? kCopperHighlight : kPvcHighlight; }

// Draws the plumbing shape using a thick outer silhouette, a material body,
// and a thin highlight. The layered construction gives even simple filled
// rectangles a clear pipe-like appearance on the 800x480 display.
void drawPieceShape(GfxCompat *g, int x, int y, int size, uint8_t mask,
                    uint16_t color, int thicknessOverride = 0) {
    const int t = thicknessOverride > 0 ? thicknessOverride : std::max(4, size / 3);
    const int cx = x + size / 2;
    const int cy = y + size / 2;
    g->fillRect(cx - t / 2, cy - t / 2, t, t, color);
    if (mask & DIR_UP) g->fillRect(cx - t / 2, y, t, size / 2 + t / 2, color);
    if (mask & DIR_DOWN) g->fillRect(cx - t / 2, cy, t, size / 2 + t / 2, color);
    if (mask & DIR_LEFT) g->fillRect(x, cy - t / 2, size / 2 + t / 2, t, color);
    if (mask & DIR_RIGHT) g->fillRect(cx, cy - t / 2, size / 2 + t / 2, t, color);
}

void drawPipe(GfxCompat *g, int x, int y, int size, uint8_t mask, Material material,
              bool filled, bool leaking) {
    const int outer = std::max(10, size / 3 + 8);
    const int body = std::max(8, size / 3);

    drawPieceShape(g, x, y + 2, size, mask, kPipeShadow, outer);
    drawPieceShape(g, x, y, size, mask, materialColor(material), body);

    // A narrow highlight makes the empty pipe visibly different from the tile.
    drawPieceShape(g, x, y - 1, size, mask, materialHighlight(material), 4);

    if (filled) {
        const uint16_t water = leaking ? kWaterLeak : kWater;
        const int waterT = std::max(5, body / 2);
        drawPieceShape(g, x, y, size, mask, water, waterT);
        if (!leaking) {
            drawPieceShape(g, x, y - 1, size, mask, kWaterHighlight, 2);
        }
    }
}

void drawCell(GfxCompat *g, Board &board, int col_, int row, const SimState &sim) {
    Cell &c = board.at(col_, row);
    if (!c.dirty) return;

    const Rect r = cellRect(col_, row);
    const bool checker = ((col_ + row) & 1) != 0;
    g->fillRect(r.x, r.y, r.w, r.h, checker ? kBoardBgB : kBoardBgA);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 3, kGrid);

    const int idx = Board::index(col_, row);

    if (c.type == PieceType::Source) {
        drawPieceShape(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation), kPipeShadow, 25);
        drawPieceShape(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation), kSource, 19);
        g->fillCircle(r.x + r.w / 2, r.y + r.h / 2, r.w / 5, kSource);
    } else if (c.type == PieceType::Target) {
        drawPieceShape(g, r.x, r.y, r.w, connectionMask(c.type, c.rotation), kTarget, 25);
        const int pad = r.w / 4;
        g->drawRoundRect(r.x + pad, r.y + pad, r.w - 2 * pad, r.h - 2 * pad, 4, kTarget);
        const int fillH = (c.volume * (r.h - 2 * pad - 4)) / PIPE_CAPACITY;
        if (fillH > 0) {
            g->fillRect(r.x + pad + 2, r.y + r.h - pad - 2 - fillH,
                        r.w - 2 * pad - 4, fillH, kWater);
        }
    } else if (c.type != PieceType::Empty) {
        const uint8_t mask = connectionMask(c.type, c.rotation);
        drawPipe(g, r.x, r.y, r.w, mask, c.material, c.volume > 0, sim.leaking[idx]);
    }

    if (sim.leaking[idx]) {
        g->fillCircle(r.x + r.w - 9, r.y + 9, 5, kLeakDot);
        g->fillCircle(r.x + r.w - 9, r.y + 9, 2, kWhite);
    }

    c.dirty = false;
}

void drawPreviewSlot(GfxCompat *g, const Rect &r, PieceType type, uint16_t slotColor,
                     bool active, Material material = Material::PVC) {
    g->fillRoundRect(r.x, r.y, r.w, r.h, 6, slotColor);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 6, active ? kHudBorderActive : kHudBorder);
    if (type == PieceType::Empty) return;

    // Give the current piece more visual weight than the upcoming queue.
    const int size = std::min(r.w, r.h) - (active ? 12 : 16);
    const int x = r.x + (r.w - size) / 2;
    const int y = r.y + (r.h - size) / 2;
    drawPipe(g, x, y, size, baseMask(type), material, false, false);

    // Small left marker identifies the active/current slot without relying on text.
    if (active) {
        const int cy = r.y + r.h / 2;
        g->fillTriangle(r.x + 5, cy, r.x + 12, cy - 6, r.x + 12, cy + 6, kHudBorderActive);
    }
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
    // Start with a dark canvas. The board is then drawn as a bright, clearly
    // bounded construction area, eliminating the previous white lower band.
    g->fillRect(0, 0, SCREEN_W, SCREEN_H, kHudPanel);
    g->fillRoundRect(BOARD_ORIGIN_X - 2, BOARD_ORIGIN_Y - 2,
                     BOARD_PIXEL_W + 4, BOARD_PIXEL_H + 4, 5, kBoardFrame);
    g->fillRect(HUD_X - 8, 0, SCREEN_W - (HUD_X - 8), SCREEN_H, kHudPanel);
    board.markAllDirty();
}

void renderBoard(GfxCompat *g, Board &board, const SimState &sim) {
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int c = 0; c < BOARD_COLS; ++c) drawCell(g, board, c, row, sim);
    }
}

void renderHud(GfxCompat *g, const WpInventory &inv, const SimState &sim, const Level &level, Outcome outcome,
               bool removeMode, WpHudCache &cache) {
    const int removeModeInt = removeMode ? 1 : 0;
    if (removeModeInt != cache.removeMode) {
        const Rect &r = REMOVE_TOGGLE_RECT;
        g->fillRoundRect(r.x, r.y, r.w, r.h, 6, removeMode ? kDefeat : kHudSlot);
        g->drawRoundRect(r.x, r.y, r.w, r.h, 6, removeMode ? kDefeat : kHudBorder);
        const int cx = r.x + r.w / 2;
        const int cy = r.y + r.h / 2;
        if (removeMode) {
            g->drawLine(cx - 7, cy - 7, cx + 7, cy + 7, kWhite);
            g->drawLine(cx - 7, cy + 7, cx + 7, cy - 7, kWhite);
        } else {
            g->drawRoundRect(cx - 7, cy - 7, 14, 14, 7, kWhite);
            g->fillTriangle(cx + 7, cy - 7, cx + 12, cy - 12, cx + 12, cy - 2, kWhite);
        }
        cache.removeMode = removeModeInt;
    }

    if (inv.hand() != cache.hand) {
        drawPreviewSlot(g, QUEUE_RECT[0], inv.hand(), kHudSlot, true);
        cache.hand = inv.hand();
    }
    if (inv.peekUpcoming(0) != cache.upcoming0) {
        drawPreviewSlot(g, QUEUE_RECT[1], inv.peekUpcoming(0), kHudSlot, false);
        cache.upcoming0 = inv.peekUpcoming(0);
    }
    if (inv.peekUpcoming(1) != cache.upcoming1) {
        drawPreviewSlot(g, QUEUE_RECT[2], inv.peekUpcoming(1), kHudSlot, false);
        cache.upcoming1 = inv.peekUpcoming(1);
    }
    if (inv.held() != cache.held || inv.hasHeld() != cache.hasHeld) {
        drawPreviewSlot(g, HOLD_RECT, inv.hasHeld() ? inv.held() : PieceType::Empty,
                        kHudSlot, inv.hasHeld(), Material::PVC);
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
        g->fillRoundRect(RESTART_RECT.x, RESTART_RECT.y, RESTART_RECT.w, RESTART_RECT.h, 7, btnColor);
        g->drawRoundRect(RESTART_RECT.x, RESTART_RECT.y, RESTART_RECT.w, RESTART_RECT.h, 7, kHudBorder);
        const int cx = RESTART_RECT.x + RESTART_RECT.w / 2;
        const int cy = RESTART_RECT.y + RESTART_RECT.h / 2;
        const int rad = RESTART_RECT.h / 3;
        g->drawRoundRect(cx - rad, cy - rad, rad * 2, rad * 2, rad, kWhite);
        g->fillTriangle(cx + rad, cy - rad, cx + rad + 6, cy - rad - 6, cx + rad + 6, cy - rad + 6, kWhite);

        const uint16_t frame = outcome == Outcome::Victory ? kVictory : outcome == Outcome::Defeat ? kDefeat : kGrid;
        const int bw = 3;
        for (int i = 0; i < bw; ++i) {
            g->drawRoundRect(BOARD_ORIGIN_X - 1 - i, BOARD_ORIGIN_Y - 1 - i,
                             BOARD_PIXEL_W + 2 + 2 * i, BOARD_PIXEL_H + 2 + 2 * i, 4, frame);
        }
        cache.outcome = outcome;
    }
}

}  // namespace wp
