#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include "Screens.h"

class LoadingScreen {
private:
    float progress;        // 0.0 to 1.0
    float timer;           // total elapsed time
    float logoAlpha;       // logo fade in
    float titleAlpha;      // title fade in
    float taglineAlpha;    // tagline fade in
    float circleAngle;     // for animated circle drawing
    int   loadingStep;     // which loading text to show
    float stepTimer;       // time on current step
    bool  done;            // true when loading complete

    // particle system — floating gold dots
    struct Particle {
        float x, y;
        float speedY;
        float alpha;
        float size;
        float alphaSpeed;
    };
    Particle particles[40];

    void initParticles();
    void updateParticles();
    void drawParticles();

public:
    LoadingScreen();
    bool isDone() const;
    void update();
    void draw();
};

#endif