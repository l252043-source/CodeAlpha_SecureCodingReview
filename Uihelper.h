#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#include <cstring>
#include "Colors.h"

inline bool MouseOver(int x, int y, int w, int h)
{ Vector2 m=GetMousePosition(); return m.x>=x&&m.x<=x+w&&m.y>=y&&m.y<=y+h; }

inline bool Clicked()
{ return IsMouseButtonPressed(MOUSE_LEFT_BUTTON); }

inline bool Button(int x, int y, int w, int h,
                   const char* lbl, Color col, int fs=16)
{
    bool hov=MouseOver(x,y,w,h);
    DrawRectangle(x,y,w,h, hov?Fade(col,0.75f):col);
    DrawRectangleLines(x,y,w,h, Fade(WHITE,0.15f));
    int tw=MeasureText(lbl,fs);
    DrawText(lbl, x+(w-tw)/2, y+(h-fs)/2, fs, BG);
    return hov&&Clicked();
}

inline bool OutlineBtn(int x, int y, int w, int h,
                       const char* lbl, Color col, int fs=14)
{
    bool hov=MouseOver(x,y,w,h);
    DrawRectangle(x,y,w,h, hov?Fade(col,0.18f):Fade(col,0.07f));
    DrawRectangleLines(x,y,w,h,col);
    int tw=MeasureText(lbl,fs);
    DrawText(lbl, x+(w-tw)/2, y+(h-fs)/2, fs, col);
    return hov&&Clicked();
}

inline void InputBox(char* buf, bool* active, int* tick,
                     int x, int y, int w, int h, const char* ph)
{
    if(Clicked()) *active=MouseOver(x,y,w,h);
    DrawRectangle(x,y,w,h, Fade(BG,0.85f));
    DrawRectangleLines(x,y,w,h, *active?GOLD:MUTED);
    if(strlen(buf)==0) DrawText(ph,  x+10, y+(h-14)/2, 14, MUTED);
    else               DrawText(buf, x+10, y+(h-14)/2, 14, SOFT_WHT);
    if(*active&&(*tick/30)%2==0)
    { int tw=MeasureText(buf,14); DrawText("|",x+10+tw,y+(h-14)/2,14,GOLD); }
    if(*active)
    {
        (*tick)++;
        int key=GetCharPressed();
        while(key>0)
        { int l=(int)strlen(buf); if(key>=32&&l<60){buf[l]=(char)key;buf[l+1]='\0';} key=GetCharPressed(); }
        if(IsKeyPressed(KEY_BACKSPACE)&&strlen(buf)>0) buf[strlen(buf)-1]='\0';
    }
}

inline bool Toggle(int x, int y, int w, int h,
                   const char* lbl, bool on, Color col)
{
    bool hov=MouseOver(x,y,w,h);
    DrawRectangle(x,y,w,h, on?col:hov?Fade(col,0.18f):Fade(col,0.07f));
    DrawRectangleLines(x,y,w,h, on?col:Fade(col,0.5f));
    int tw=MeasureText(lbl,13);
    DrawText(lbl, x+(w-tw)/2, y+(h-13)/2, 13, on?BG:col);
    if(hov&&Clicked()) return !on;
    return on;
}

inline void TopBar(const char* title, bool showBack=false)
{
    int sw=GetScreenWidth();
    DrawRectangle(0,0,sw,56,PANEL);
    DrawRectangle(0,53,sw,3,GOLD);
    DrawText("DINE",16,14,28,GOLD);
    DrawText("360", 78,18,20,SOFT_WHT);
    int tw=MeasureText(title,18);
    DrawText(title, sw/2-tw/2, 18, 18, SOFT_WHT);
    if(showBack) OutlineBtn(sw-120,12,104,32,"< Back",GOLD,13);
}

inline bool BackClicked()
{ return MouseOver(GetScreenWidth()-120,12,104,32)&&Clicked(); }

inline void Heading(const char* txt, int x, int y, int fs=13)
{
    DrawText(txt,x,y,fs,GOLD);
    DrawLine(x,y+fs+3, x+MeasureText(txt,fs)+30, y+fs+3, Fade(GOLD,0.3f));
}

inline void DrawBG()
{
    ClearBackground(BG);
    for(int i=0;i<GetScreenHeight();i+=6)
        DrawLine(0,i,GetScreenWidth(),i,Fade(PANEL,0.25f));
}