#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#include "Auth.h"

// =====================
// LUXURY DARK THEME
// =====================
#define BG_COLOR        Color{13, 27, 42, 255}
#define PANEL_COLOR     Color{26, 46, 69, 255}
#define GOLD            Color{224, 201, 127, 255}
#define GOLD_DIM        Color{160, 140, 80, 255}
#define INPUT_BG        Color{20, 36, 54, 255}
#define INPUT_BORDER    Color{42, 64, 96, 255}
#define INPUT_ACTIVE    Color{60, 90, 130, 255}
#define TEXT_MUTED      Color{138, 155, 176, 255}
#define TEXT_DIM        Color{74, 96, 128, 255}
#define ERROR_RED       Color{220, 80, 80, 255}
#define SUCCESS_GREEN   Color{80, 200, 140, 255}
#define WHITE_COLOR     Color{255, 255, 255, 255}
#define STRENGTH_WEAK   Color{220, 80, 80, 255}
#define STRENGTH_MED    Color{220, 180, 60, 255}
#define STRENGTH_STRONG Color{80, 200, 140, 255}

// =====================
// SCREEN IDENTIFIERS
// =====================
enum Screen {
    SCREEN_LOADING,
    SCREEN_LOGIN,
    SCREEN_SIGNUP,
    SCREEN_CUSTOMER_HOME,
    SCREEN_STAFF_DASHBOARD,
    SCREEN_MENU,
    SCREEN_INVENTORY
};
// =====================
// GLOBAL TRANSITION
// =====================
// Call these in main.cpp
// transitionAlpha goes 0->1 (fade out) then on new screen 1->0 (fade in)

extern float transitionAlpha;
extern bool  transitionFadingOut;
extern Screen transitionTarget;
extern bool  transitionActive;

// Call this to trigger a transition to a new screen
inline void StartTransition(Screen target) {
    transitionTarget = target;
    transitionFadingOut = true;
    transitionActive = true;
}

// Call this every frame in main.cpp to update transition state
// Returns the new screen when transition is complete
inline Screen UpdateTransition(Screen current) {
    if (!transitionActive) return current;

    float speed = 2.5f * GetFrameTime();

    if (transitionFadingOut) {
        transitionAlpha += speed;
        if (transitionAlpha >= 1.0f) {
            transitionAlpha = 1.0f;
            transitionFadingOut = false;
            return transitionTarget; // switch screen at peak black
        }
    }
    else {
        transitionAlpha -= speed;
        if (transitionAlpha <= 0.0f) {
            transitionAlpha = 0.0f;
            transitionActive = false;
        }
    }
    return current;
}

// Call this at the END of BeginDrawing() block every frame
inline void DrawTransitionOverlay() {
    if (!transitionActive && transitionAlpha <= 0.0f) return;
    int alpha = (int)(transitionAlpha * 255);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
        Color{ 0, 0, 0, (unsigned char)alpha });
}

// Fonts declared here
extern Font fontRegular;
extern Font fontBold;

#endif