#include "wp_menu.h"

#include "core/GfxCompat.h"

namespace wp {
namespace {

constexpr uint16_t kBg = GfxCompat::color565(18, 22, 28);
constexpr uint16_t kPanel = GfxCompat::color565(29, 35, 43);
constexpr uint16_t kPanel2 = GfxCompat::color565(38, 46, 56);
constexpr uint16_t kText = GfxCompat::color565(235, 241, 246);
constexpr uint16_t kMuted = GfxCompat::color565(145, 157, 169);
constexpr uint16_t kBlue = GfxCompat::color565(54, 170, 235);
constexpr uint16_t kBlueDark = GfxCompat::color565(29, 89, 126);
constexpr uint16_t kGold = GfxCompat::color565(247, 194, 68);
constexpr uint16_t kLocked = GfxCompat::color565(67, 75, 84);

void glyph(char c, uint8_t rows[7]) {
    for (int i = 0; i < 7; ++i) rows[i] = 0;
    switch (c) {
        case 'A': { const uint8_t r[] = {14,17,17,31,17,17,17}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'B': { const uint8_t r[] = {30,17,17,30,17,17,30}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'C': { const uint8_t r[] = {14,17,16,16,16,17,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'D': { const uint8_t r[] = {30,17,17,17,17,17,30}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'E': { const uint8_t r[] = {31,16,16,30,16,16,31}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'F': { const uint8_t r[] = {31,16,16,30,16,16,16}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'G': { const uint8_t r[] = {14,17,16,23,17,17,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'H': { const uint8_t r[] = {17,17,17,31,17,17,17}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'I': { const uint8_t r[] = {31,4,4,4,4,4,31}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'J': { const uint8_t r[] = {7,2,2,2,18,18,12}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'K': { const uint8_t r[] = {17,18,20,24,20,18,17}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'L': { const uint8_t r[] = {16,16,16,16,16,16,31}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'M': { const uint8_t r[] = {17,27,21,21,17,17,17}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'N': { const uint8_t r[] = {17,25,21,19,17,17,17}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'O': { const uint8_t r[] = {14,17,17,17,17,17,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'P': { const uint8_t r[] = {30,17,17,30,16,16,16}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'Q': { const uint8_t r[] = {14,17,17,17,21,18,13}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'R': { const uint8_t r[] = {30,17,17,30,20,18,17}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'S': { const uint8_t r[] = {15,16,16,14,1,1,30}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'T': { const uint8_t r[] = {31,4,4,4,4,4,4}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'U': { const uint8_t r[] = {17,17,17,17,17,17,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'V': { const uint8_t r[] = {17,17,17,17,17,10,4}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'W': { const uint8_t r[] = {17,17,17,21,21,21,10}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'X': { const uint8_t r[] = {17,17,10,4,10,17,17}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'Y': { const uint8_t r[] = {17,17,10,4,4,4,4}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case 'Z': { const uint8_t r[] = {31,1,2,4,8,16,31}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '0': { const uint8_t r[] = {14,17,19,21,25,17,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '1': { const uint8_t r[] = {4,12,4,4,4,4,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '2': { const uint8_t r[] = {14,17,1,2,4,8,31}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '3': { const uint8_t r[] = {30,1,1,14,1,1,30}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '4': { const uint8_t r[] = {2,6,10,18,31,2,2}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '5': { const uint8_t r[] = {31,16,16,30,1,1,30}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '6': { const uint8_t r[] = {14,16,16,30,17,17,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '7': { const uint8_t r[] = {31,1,2,4,8,8,8}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '8': { const uint8_t r[] = {14,17,17,14,17,17,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case '9': { const uint8_t r[] = {14,17,17,15,1,1,14}; for(int i=0;i<7;++i)rows[i]=r[i]; break; }
        case ':': rows[2]=4; rows[4]=4; break;
        case '-': rows[3]=14; break;
        case '*': rows[1]=4; rows[2]=21; rows[3]=14; rows[4]=21; rows[5]=4; break;
        case '>': rows[2]=16; rows[3]=8; rows[4]=16; break;
        case '<': rows[2]=1; rows[3]=2; rows[4]=1; break;
        default: break;
    }
}

int textWidth(const char *text, int scale) {
    int n = 0;
    for (const char *p = text; *p; ++p) ++n;
    return n ? n * (6 * scale) - scale : 0;
}

void drawText(GfxCompat *g, int x, int y, const char *text, int scale, uint16_t color) {
    for (const char *p = text; *p; ++p) {
        uint8_t rows[7];
        glyph(*p, rows);
        for (int ry = 0; ry < 7; ++ry) {
            for (int rx = 0; rx < 5; ++rx) {
                if (rows[ry] & (1u << (4 - rx))) {
                    g->fillRect(x + rx * scale, y + ry * scale, scale, scale, color);
                }
            }
        }
        x += 6 * scale;
    }
}

void centeredText(GfxCompat *g, int y, const char *text, int scale, uint16_t color) {
    drawText(g, (800 - textWidth(text, scale)) / 2, y, text, scale, color);
}

void button(GfxCompat *g, const MenuRect &r, const char *label, bool enabled, bool primary) {
    const uint16_t fill = !enabled ? kLocked : (primary ? kBlueDark : kPanel2);
    const uint16_t edge = !enabled ? kLocked : (primary ? kBlue : kMuted);
    g->fillRoundRect(r.x, r.y, r.w, r.h, 10, fill);
    g->drawRoundRect(r.x, r.y, r.w, r.h, 10, edge);
    const int scale = 3;
    const int tw = textWidth(label, scale);
    drawText(g, r.x + (r.w - tw) / 2, r.y + (r.h - 21) / 2, label, scale,
             enabled ? kText : kMuted);
}

void stars(GfxCompat *g, int x, int y, int count) {
    for (int i = 0; i < 3; ++i) {
        drawText(g, x + i * 28, y, "*", 3, i < count ? kGold : kMuted);
    }
}

void drawPipeLogo(GfxCompat *g) {
    const int x = 315, y = 68;
    g->fillRoundRect(x, y + 12, 170, 34, 12, kBlueDark);
    g->fillRoundRect(x + 68, y - 4, 34, 66, 12, kBlue);
    g->fillCircle(x + 85, y + 29, 8, kText);
}

void renderMain(GfxCompat *g, const ProgressData &p) {
    g->fillScreen(kBg);
    drawPipeLogo(g);
    centeredText(g, 18, "WATER PIPE", 4, kText);
    centeredText(g, 112, "BUILD - FLOW - SURVIVE", 2, kMuted);

    button(g, MENU_CONTINUE, "CONTINUE", p.resume.valid, true);
    button(g, MENU_PHASES, "PHASES", true, false);
    button(g, MENU_SCORES, "SCORES", true, false);

    centeredText(g, 325, "STARS", 2, kMuted);
    stars(g, 346, 349, [&](){ int s=0; for(int i=0;i<MAX_PHASES;++i)s+=p.stars[i]; return s; }());

    if (p.highScores[0].score > 0) {
        centeredText(g, 382, "BEST", 2, kMuted);
        char score[16];
        int n = p.highScores[0].score;
        int pos = 0;
        if (n == 0) score[pos++]='0';
        else { char rev[12]; int rn=0; while(n>0){rev[rn++]=char('0'+n%10);n/=10;} while(rn)score[pos++]=rev[--rn]; }
        score[pos]=0;
        centeredText(g, 401, score, 3, kGold);
    }
}

void renderPhases(GfxCompat *g, const ProgressData &p) {
    g->fillScreen(kBg);
    centeredText(g, 24, "PHASES", 4, kText);
    centeredText(g, 72, "SELECT A PHASE", 2, kMuted);
    for (int i = 0; i < 6; ++i) {
        const MenuRect &r = MENU_PHASE_BUTTONS[i];
        const bool unlocked = i <= p.unlockedPhase;
        g->fillRoundRect(r.x, r.y, r.w, r.h, 10, unlocked ? kPanel2 : kPanel);
        g->drawRoundRect(r.x, r.y, r.w, r.h, 10, unlocked ? kBlue : kLocked);
        char num[2] = {char('1' + i), 0};
        drawText(g, r.x + 31, r.y + 12, num, 5, unlocked ? kText : kMuted);
        if (unlocked) stars(g, r.x + 12, r.y + 50, p.stars[i]);
        else centeredText(g, r.y + 50, "LOCKED", 1, kMuted);
    }
    button(g, MENU_BACK, "BACK", true, false);
}

void renderScores(GfxCompat *g, const ProgressData &p) {
    g->fillScreen(kBg);
    centeredText(g, 24, "HIGH SCORES", 4, kText);
    for (int i = 0; i < MAX_HIGHSCORES; ++i) {
        const int y = 105 + i * 52;
        g->fillRoundRect(150, y, 500, 40, 8, i == 0 ? kPanel2 : kPanel);
        char rank[2] = {char('1' + i), 0};
        drawText(g, 170, y + 11, rank, 2, kMuted);
        if (p.highScores[i].score <= 0) {
            drawText(g, 240, y + 11, "EMPTY", 2, kMuted);
            continue;
        }
        char phase[2] = {char('1' + p.highScores[i].phase), 0};
        drawText(g, 225, y + 11, "PHASE", 2, kMuted);
        drawText(g, 265, y + 11, phase, 2, kText);
        int n = p.highScores[i].score;
        char rev[12]; int rn=0; while(n>0){rev[rn++]=char('0'+n%10);n/=10;}
        char score[12]; int pos=0; while(rn)score[pos++]=rev[--rn]; score[pos]=0;
        drawText(g, 500, y + 11, score, 2, kGold);
    }
    button(g, MENU_BACK, "BACK", true, false);
}

}  // namespace

void renderMenu(GfxCompat *g, MenuScreen screen, const ProgressData &progress) {
    if (!g) return;
    switch (screen) {
        case MenuScreen::Main: renderMain(g, progress); break;
        case MenuScreen::Phases: renderPhases(g, progress); break;
        case MenuScreen::Scores: renderScores(g, progress); break;
    }
}

}  // namespace wp
