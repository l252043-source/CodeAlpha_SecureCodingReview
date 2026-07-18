#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>

#ifdef GOLD
#undef GOLD
#endif
#ifdef GREEN
#undef GREEN
#endif
#ifdef RED
#undef RED
#endif

namespace AppColors
{
    inline Color MkCol(unsigned char r, unsigned char g,
        unsigned char b, unsigned char a)
    {
        Color c; c.r = r; c.g = g; c.b = b; c.a = a; return c;
    }

    const Color BG = { 18,  20,  35, 255 };
    const Color PANEL = { 28,  32,  52, 255 };
    const Color GOLD = { 210, 170,  60, 255 };
    const Color SOFT_WHT = { 220, 220, 230, 255 };
    const Color MUTED = { 130, 130, 150, 255 };
    const Color GREEN = { 60, 190,  90, 255 };
    const Color RED_C = { 200,  60,  60, 255 };
    const Color TEAL = { 50, 180, 170, 255 };
}

// Bring all colors into global scope 
using AppColors::BG;
using AppColors::PANEL;
using AppColors::GOLD;
using AppColors::SOFT_WHT;
using AppColors::MUTED;
using AppColors::GREEN;
using AppColors::RED_C;
using AppColors::TEAL;
using AppColors::MkCol;