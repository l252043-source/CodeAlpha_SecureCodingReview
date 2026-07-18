#include "LOADINGSCREEN.h"
#include <cstdlib>

extern Font fontRegular;
extern Font fontBold;

// loading step messages
static const char* LOAD_STEPS[] = {
    "Initializing system...",
    "Loading menu data...",
    "Preparing tables...",
    "Setting the ambience...",
    "Welcome to DINE 360"
};
static const int LOAD_STEP_COUNT = 5;

// ================================
// PARTICLES
// ================================

void LoadingScreen::initParticles() {
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    for (int i = 0; i < 40; i++) {
        particles[i].x = (float)(rand() % W);
        particles[i].y = (float)(rand() % H);
        particles[i].speedY = 0.2f + (rand() % 10) * 0.05f;
        particles[i].alpha = 0.1f + (rand() % 6) * 0.05f;
        particles[i].size = 1.0f + (rand() % 3);
        particles[i].alphaSpeed = 0.005f + (rand() % 5) * 0.002f;
    }
}

void LoadingScreen::updateParticles() {
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    for (int i = 0; i < 40; i++) {
        particles[i].y -= particles[i].speedY;
        particles[i].alpha += particles[i].alphaSpeed;

        // reverse alpha direction for twinkle
        if (particles[i].alpha > 0.6f || particles[i].alpha < 0.05f)
            particles[i].alphaSpeed = -particles[i].alphaSpeed;

        // reset if off screen
        if (particles[i].y < -5) {
            particles[i].y = (float)H + 5;
            particles[i].x = (float)(rand() % W);
        }
    }
}

void LoadingScreen::drawParticles() {
    for (int i = 0; i < 40; i++) {
        unsigned char a = (unsigned char)(particles[i].alpha * 255);
        DrawCircle(
            (int)particles[i].x,
            (int)particles[i].y,
            particles[i].size,
            Color{ 224, 201, 127, a }
        );
    }
}

// ================================
// CONSTRUCTOR
// ================================

LoadingScreen::LoadingScreen() {
    progress = 0.0f;
    timer = 0.0f;
    logoAlpha = 0.0f;
    titleAlpha = 0.0f;
    taglineAlpha = 0.0f;
    circleAngle = 0.0f;
    loadingStep = 0;
    stepTimer = 0.0f;
    done = false;
    initParticles();
}

bool LoadingScreen::isDone() const {
    return done;
}

// ================================
// UPDATE
// ================================

void LoadingScreen::update() {
    if (done) return;

    float dt = GetFrameTime();
    timer += dt;

    updateParticles();

    // logo circle draws itself in first 0.8 seconds
    if (timer < 0.8f)
        circleAngle = (timer / 0.8f) * 360.0f;
    else
        circleAngle = 360.0f;

    // logo alpha fades in 0->0.6s
    if (timer < 0.6f)
        logoAlpha = timer / 0.6f;
    else
        logoAlpha = 1.0f;

    // title fades in 0.5->1.1s
    if (timer > 0.5f && timer < 1.1f)
        titleAlpha = (timer - 0.5f) / 0.6f;
    else if (timer >= 1.1f)
        titleAlpha = 1.0f;

    // tagline fades in 0.9->1.4s
    if (timer > 0.9f && timer < 1.4f)
        taglineAlpha = (timer - 0.9f) / 0.5f;
    else if (timer >= 1.4f)
        taglineAlpha = 1.0f;

    // progress bar fills over 3.5 seconds total
    // starts at 1.2s
    if (timer > 1.2f) {
        float elapsed = timer - 1.2f;
        progress = elapsed / 3.5f;
        if (progress > 1.0f) progress = 1.0f;
    }

    // loading step cycling
    stepTimer += dt;
    float stepDuration = 3.5f / LOAD_STEP_COUNT;
    if (stepTimer >= stepDuration && loadingStep < LOAD_STEP_COUNT - 1) {
        loadingStep++;
        stepTimer = 0.0f;
    }

    // done after 5.0 seconds
    if (timer >= 5.0f)
        done = true;
}

// ================================
// DRAW
// ================================

void LoadingScreen::draw() {
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float cx = W / 2.0f;
    float cy = H / 2.0f - 60;

    ClearBackground(BG_COLOR);

    // gold top and bottom accent lines
    DrawRectangle(0, 0, W, 3, GOLD);
    DrawRectangle(0, H - 3, W, 3, GOLD);

    // floating particles
    drawParticles();

    // ---- LOGO CIRCLE ----
    unsigned char logoA = (unsigned char)(logoAlpha * 255);

    // outer ring — draws progressively
    int segments = (int)(circleAngle / 360.0f * 60);
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / 60.0f * 360.0f * DEG2RAD - 90 * DEG2RAD;
        float angle2 = (float)(i + 1) / 60.0f * 360.0f * DEG2RAD - 90 * DEG2RAD;
        float r = 64.0f;
        DrawLineEx(
            { cx + r * cosf(angle1), cy + r * sinf(angle1) },
            { cx + r * cosf(angle2), cy + r * sinf(angle2) },
            2.5f,
            Color{ 224, 201, 127, logoA }
        );
    }

    // inner circle fill
    DrawCircle((int)cx, (int)cy, 58,
        Color{ 26, 46, 69, logoA });

    // second decorative ring
    DrawCircleLines((int)cx, (int)cy, 52,
        Color{ 42, 64, 96, logoA });

    // 360 badge text inside
    const char* badge = "360";
    float bs = 28.0f;
    Vector2 bm = MeasureTextEx(fontBold, badge, bs, 1);
    DrawTextEx(fontBold, badge,
        { cx - bm.x / 2 - 4, cy - bm.y / 2 },
        bs, 1, Color{ 224, 201, 127, logoA });
    // degree superscript
    DrawTextEx(fontBold, "o",
        { cx + bm.x / 2 - 2, cy - bm.y / 2 - 5 },
        14, 1, Color{ 224, 201, 127, logoA });

    // ---- TITLE ----
    unsigned char titleA = (unsigned char)(titleAlpha * 255);
    const char* title = "DINE 360";
    float ts = 56.0f;
    Vector2 tm = MeasureTextEx(fontBold, title, ts, 1);
    DrawTextEx(fontBold, title,
        { cx - tm.x / 2 - 8, cy + 80 },
        ts, 1, Color{ 224, 201, 127, titleA });
    DrawTextEx(fontBold, "o",
        { cx + tm.x / 2 - 8, cy + 74 },
        24, 1, Color{ 224, 201, 127, titleA });

    // ---- TAGLINE ----
    unsigned char tagA = (unsigned char)(taglineAlpha * 255);
    const char* tagline =
        "Classic Fine Dining  |  Reserve  *  Celebrate  *  Remember";
    float tgs = 14.0f;
    Vector2 tgm = MeasureTextEx(fontRegular, tagline, tgs, 1);
    DrawTextEx(fontRegular, tagline,
        { cx - tgm.x / 2, cy + 148 },
        tgs, 1, Color{ 138, 155, 176, tagA });

    // decorative divider lines around tagline
    float divY = cy + 144;
    DrawRectangle((int)(cx - tgm.x / 2 - 30), (int)divY, 24, 1,
        Color{ 224, 201, 127, tagA });
    DrawRectangle((int)(cx + tgm.x / 2 + 6), (int)divY, 24, 1,
        Color{ 224, 201, 127, tagA });

    // ---- PROGRESS BAR ----
    float barW = W * 0.5f;
    float barH = 4.0f;
    float barX = cx - barW / 2;
    float barY = (float)(H - 60);

    // bar background
    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH,
        Color{ 42, 64, 96, 255 });

    // bar fill — gold with glow effect
    float fillW = barW * progress;
    DrawRectangle((int)barX, (int)barY, (int)fillW, (int)barH, GOLD);

    // glow at bar end
    if (fillW > 4) {
        DrawRectangle(
            (int)(barX + fillW - 4), (int)(barY - 2),
            8, (int)(barH + 4),
            Color{ 255, 230, 150, 180 });
    }

    // progress percentage
    char pctStr[8];
    int pct = (int)(progress * 100);
    pctStr[0] = '0' + pct / 100;
    pctStr[1] = '0' + (pct % 100) / 10;
    pctStr[2] = '0' + pct % 10;
    pctStr[3] = '%'; pctStr[4] = 0;
    // trim leading zeros
    const char* pctDisplay = pctStr;
    if (pctStr[0] == '0' && pctStr[1] != '%') {
        pctDisplay = pctStr + 1;
        if (pctStr[1] == '0' && pctStr[2] != '%')
            pctDisplay = pctStr + 2;
    }
    Vector2 pm = MeasureTextEx(fontRegular, pctDisplay, 12, 1);
    DrawTextEx(fontRegular, pctDisplay,
        { barX + barW + 10, barY - 4 },
        12, 1, TEXT_MUTED);

    // ---- LOADING STEP TEXT ----
    if (loadingStep < LOAD_STEP_COUNT) {
        const char* stepTxt = LOAD_STEPS[loadingStep];
        Vector2 sm = MeasureTextEx(fontRegular, stepTxt, 13, 1);

        // fade in/out based on step timer
        float stepDuration = 3.5f / LOAD_STEP_COUNT;
        float stepAlpha = 1.0f;
        if (stepTimer < 0.2f)
            stepAlpha = stepTimer / 0.2f;
        else if (stepTimer > stepDuration - 0.2f)
            stepAlpha = (stepDuration - stepTimer) / 0.2f;
        if (stepAlpha < 0) stepAlpha = 0;
        if (stepAlpha > 1) stepAlpha = 1;

        DrawTextEx(fontRegular, stepTxt,
            { cx - sm.x / 2, barY - 28 },
            13, 1, Color{ 138, 155, 176, (unsigned char)(stepAlpha * 255) });
    }

    // ---- BOTTOM CREDIT ----
    const char* credit = "Team 4  |  BCY-2A  |  FAST-NUCES  |  OOP Project";
    Vector2 cm = MeasureTextEx(fontRegular, credit, 11, 1);
    DrawTextEx(fontRegular, credit,
        { cx - cm.x / 2, (float)(H - 20) },
        11, 1, Color{ 74, 96, 128, 200 });
}