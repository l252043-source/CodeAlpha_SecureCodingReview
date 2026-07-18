// Muhammad Muneeb (25L-2043)
//  Team 4   BCY-2A   OOP Project   FAST-NUCES
// ============================================================
#define _CRT_SECURE_NO_WARNINGS
#include "ReservationManager.h"
#include "UIHelper.h"
#include "Colors.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime>

static const char* table_name[12] = {
    "W-1","W-2","W-3","I-1","I-2","I-3","I-4","O-1","O-2","O-3","P-1","P-2"
};
static const char* table_type[12] = {
    "Window","Window","Window","Indoor","Indoor","Indoor","Indoor","Outdoor","Outdoor","Outdoor","Private","Private"
};
static const int table_capacity[12] = { 4,4,4, 6,6,6,6, 8,8,8, 14,14 };

static const char* event_type[3] = { "Birthday","Anniversary","Corporate" };
static const char* event_package[4] = { "Basic","Business","Premium","Corporate" };
static const int   event_fee[4] = { 5000, 8000, 25000, 20000 };
//Date and Time Validation
static bool parseDate(const char* str, int& day, int& mon, int& yr){
    if ((int)strlen(str) != 10) {
        return false;
    }
    if (str[2] != '/' || str[5] != '/') {
        return false;
    }
    day = (str[0] - '0') * 10 + (str[1] - '0');
    mon = (str[3] - '0') * 10 + (str[4] - '0');
    yr = (str[6] - '0') * 1000 + (str[7] - '0') * 100 + (str[8] - '0') * 10 + (str[9] - '0');
    if (day < 1 || day > 31) {
        return false;
    }
    if (mon < 1 || mon > 12) {
        return false;
    }
    if (yr < 2000) {
        return false;
    }
    return true;
}
static int parseTime(const char* str)
{
    int len = (int)strlen(str);
    if (len < 7) {
        return -1;
    }
    int colonPos = -1;
    for (int i = 0; i < len; i++) {
        if (str[i] == ':') {
             colonPos = i; 
             break; 
        }
    } 
    if (colonPos < 1) {
        return -1;
    }
    int hour = 0;
    int minute = 0;
    for (int i = 0; i < colonPos; i++) {
        hour = hour * 10 + (str[i] - '0');
    }
    minute = (str[colonPos + 1] - '0') * 10 + (str[colonPos + 2] - '0');

    bool isPM = false;
    bool isAM = false;

    for (int i = len - 1; i >= 0; i--) {
        if (str[i] == 'P' || str[i] == 'p') { 
            isPM = true;  break; 
        }
        if (str[i] == 'A' || str[i] == 'a') {
            isAM = true;  break; 
        }
    }
    if (!isPM && !isAM) {
        return -1;
    }
    if (isPM && hour != 12) {
        hour += 12;
    }
    if (isAM && hour == 12) {
        hour = 0;
    }
    if (hour < 0 || hour   > 23) {
        return -1;
    }
    if (minute < 0 || minute > 59) {
        return -1;
    }
    return hour * 60 + minute;
}

static bool isDatePast(const char* dateStr){
    int d, m, y;
    if (!parseDate(dateStr, d, m, y)) {
        return false;
    }
    time_t now = time(nullptr); 
    tm* today = localtime(&now);
    int ty = today->tm_year + 1900, tm2 = today->tm_mon + 1, td = today->tm_mday;
    if (y < ty) {
        return true;
    }
    if (y == ty && m < tm2) {
        return true;
    }
    if (y == ty && m == tm2 && d < td) {
        return true;
    }
    return false;
}
static bool isDateToday(const char* dateStr){
    int d, m, y;
    if (!parseDate(dateStr, d, m, y)) {
        return false;
    }
    time_t now = time(nullptr); tm* today = localtime(&now);
    return (y == today->tm_year + 1900 && m == today->tm_mon + 1 && d == today->tm_mday);
}
static bool isTimePast(const char* timeStr)
{
    int entered = parseTime(timeStr);
    if (entered < 0) {
        return false;
    }
    time_t now = time(nullptr); tm* cur = localtime(&now);
    return (entered <= cur->tm_hour * 60 + cur->tm_min);
}
static void getTodayStr(char* buf){
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    int d = t->tm_mday;
    int m = t->tm_mon + 1;
    int y = t->tm_year + 1900;
    // DD
    buf[0] = '0' + d / 10;
    buf[1] = '0' + d % 10;
    buf[2] = '/';
    // MM
    buf[3] = '0' + m / 10;
    buf[4] = '0' + m % 10;
    buf[5] = '/';
    // YYYY
    buf[6] = '0' + y / 1000;
    buf[7] = '0' + (y / 100) % 10;
    buf[8] = '0' + (y / 10) % 10;
    buf[9] = '0' + y % 10;
    buf[10] = '\0';
}

static void getNowTimeStr(char* buf){
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    int  h = t->tm_hour;
    int  mn = t->tm_min;
    bool pm = (h >= 12);
    // Convert to 12-hour format
    if (h == 0) {
        h = 12;
    }
    else if (h > 12) {
        h -= 12;
    }
    // HH
    buf[0] = '0' + h / 10;
    buf[1] = '0' + h % 10;
    buf[2] = ':';
    // MM
    buf[3] = '0' + mn / 10;
    buf[4] = '0' + mn % 10;
    buf[5] = ' ';
    // AM / PM
    buf[6] = pm ? 'P' : 'A';
    buf[7] = 'M';
    buf[8] = '\0';
}
//Constructor
ReservationManager::ReservationManager(){
    for (int i = 0; i < 12; i++) {
        tableOccupied[i] = 0;
    }
    for (int i = 0; i < MAX_RES; i++) {
        reservation_tableIDX[i] = -1;
    }
    // Advance booking form char arrays
    for (int i = 0; i < sizeof(booking_tableType); i++) {
        booking_tableType[i] = '\0';
    }
    for (int i = 0; i < sizeof(booking_date); i++) {
        booking_date[i] = '\0';

    }    
    for (int i = 0; i < sizeof(booking_time); i++) {
        booking_time[i] = '\0';
    }       
    for (int i = 0; i < sizeof(booking_note); i++) {
        booking_note[i] = '\0';
    }        
    for (int i = 0; i < sizeof(booking_error); i++) {
        booking_error[i] = '\0';
    } 
    // Event form char arrays
    for (int i = 0; i < sizeof(event_date); i++) {
        event_date[i] = '\0';
    }
    for (int i = 0; i < sizeof(event_time); i++) {
        event_time[i] = '\0';
    }
    for (int i = 0; i < sizeof(event_guests); i++) {
        event_guests[i] = '\0';
    }
    for (int i = 0; i < sizeof(event_error); i++) {
        event_error[i] = '\0';
    }
    // Search state char arrays
    for (int i = 0; i < sizeof(search_input); i++) {
        search_input[i] = '\0';
    }
    for (int i = 0; i < sizeof(search_error); i++) {
        search_error[i] = '\0';
    }
}
ReservationManager::~ReservationManager() {}
//Getters
const char* ReservationManager::tableName(int i) const {
    if (i >= 0 && i < 12) {
        return table_name[i];
    }
    else {
        return "?";
    }
}
const char* ReservationManager::tableTypeName(int i) const {
    if (i >= 0 && i < 12) {
        return table_type[i];
    }
    else {
        return "?";
    }
}
int  ReservationManager::getResCount() const {
    return resCount; 
}
int  ReservationManager::getEvtCount() const {
    return evtCount; 
}
int ReservationManager::getOccupied() const {
    int count = 0;
    for (int i = 0; i < 12; i++) {
        if (tableOccupied[i]) {
            count++;
        }
    }
    return count;
}
bool ReservationManager::isSlotTaken(int tableIdx, const char* date, const char* time) const {
    int reqMins = parseTime(time);
    for (int i = 0; i < resCount; i++) {
        if (reservation_tableIDX[i] != tableIdx) {
            continue;
        }
        if (strcmp(reservations[i].getStatus(), "Cancelled") == 0) {
            continue;
        }
        if (strcmp(reservations[i].getDate(), date) != 0) {
            continue;
        }
        if (reqMins < 0) {
            return true;
        }
        int existMins = parseTime(reservations[i].getTime());
        if (existMins < 0) {
            return true;
        }
        int diff = reqMins - existMins;
        if (diff < 0) {
            diff = -diff;
        }
        if (diff < 120) {
            return true;
        }
    }
    return false;
}
//Add helpers
void ReservationManager::addReservation(int tableIdx, const char* tType,const char* date, const char* time,int guests, int pkg){
    if (resCount >= MAX_RES) {
        return;
    }
    Reservation newRes(nextID, tType, date, time, guests);
    nextID++;
    reservations[resCount] = newRes;
    tableOccupied[tableIdx] = 1;
    reservation_tableIDX[resCount] = tableIdx;
    resCount++;
}
void ReservationManager::addEvent(const char* eType, const char* date,const char* time, int guests, int pkg,bool deco, bool menu, bool dj){
    if (evtCount >= MAX_EVT) {
        return;
    }
    Event newEvent(nextID, "Private", date, time, guests,
        eType, pkg, deco, menu, dj);
    nextID++;
    events[evtCount] = newEvent;
    evtCount++;
}
//Cancel helpers
void ReservationManager::cancelReservation(int idx) {
    if (idx < 0 || idx >= resCount) {
        return;
    }
    if (strcmp(reservations[idx].getStatus(), "Cancelled") == 0) {
        return;
    }
    reservations[idx].cancel();
    int tableIndex = reservation_tableIDX[idx];
    if (tableIndex >= 0 && tableIndex < 12) {
        tableOccupied[tableIndex] = 0;
    }
    cancelConfirmRes = -1;
    search_cancelConfirmRes = -1;
}
void ReservationManager::cancelEvent(int idx){
    if (idx < 0 || idx >= evtCount) {
        return;
    }
    if (strcmp(events[idx].getStatus(), "Cancelled") == 0) {
        return;
    }
    events[idx].cancel();
    cancelConfirmEvt = -1;
    search_cancelConfirmEvt = -1;
}

//Mode selection screen
void ReservationManager::drawModeSelect(){
    DrawBG();
    int screen_width = GetScreenWidth(), screen_height = GetScreenHeight();
    float time = (float)GetTime();

    DrawRectangle(0, 0, screen_width, 58, PANEL);
    DrawRectangle(0, 55, screen_width, 3, GOLD);
    DrawText("DINE", 20, 12, 32, GOLD);
    DrawText("360", 84, 17, 22, SOFT_WHT);
    const char* tag = "Classic Fine Dining";
    DrawText(tag, screen_width / 2 - MeasureText(tag, 14) / 2, 20, 14, MUTED);
    DrawCircle(screen_width - 26, 28, 5.0f + 1.4f * sinf(time * 2.0f), Fade(GREEN, 0.85f));
    DrawText("LIVE", screen_width - 54, 22, 10, Fade(GREEN, 0.65f));
    const char* wh = "Welcome to DINE 360";
    const char* sub = "How would you like to proceed today?";
    DrawText(wh, screen_width / 2 - MeasureText(wh, 26) / 2, 68, 26, SOFT_WHT);
    DrawText(sub, screen_width / 2 - MeasureText(sub, 13) / 2, 102, 13, MUTED);
    DrawLine(screen_width / 2 - 200, 126, screen_width / 2 - 10, 126, Fade(GOLD, 0.25f));
    DrawCircle(screen_width / 2, 126, 3, Fade(GOLD, 0.6f));
    DrawLine(screen_width / 2 + 10, 126, screen_width / 2 + 200, 126, Fade(GOLD, 0.25f));
    int findBarH = 56;
    int cardAreaH = screen_height - 170 - findBarH - 8;
    int margin = 60, gap = 24;
    int totalW = screen_width - margin * 2;
    int cardW = (totalW - gap) / 2;
    int card1x = margin, card2x = margin + cardW + gap;
    int cardY = 138, cardH = cardAreaH;
    //Card-1 Dine-IN
    {
        int cx = card1x;
        bool hov = MouseOver(cx, cardY, cardW, cardH);
        DrawRectangle(cx + 4, cardY + 4, cardW, cardH, Fade(TEAL, 0.06f));
        DrawRectangle(cx, cardY, cardW, cardH, Fade(PANEL, hov ? 0.97f : 0.90f));
        DrawRectangleLines(cx, cardY, cardW, cardH, Fade(TEAL, hov ? 0.85f : 0.30f));
        DrawRectangle(cx, cardY, cardW, 5, TEAL);
        DrawRectangle(cx, cardY, 4, cardH, Fade(TEAL, 0.55f));

        int icx = cx + cardW / 2, icy = cardY + 90;
        DrawCircle(icx, icy, 40, Fade(TEAL, 0.10f));
        DrawCircleLines(icx, icy, 40, Fade(TEAL, 0.40f));
        DrawLineEx({ (float)(icx - 14),(float)(icy - 24) }, { (float)(icx - 14),(float)(icy + 24) }, 2.5f, Fade(TEAL, 0.80f));
        DrawLineEx({ (float)(icx - 14),(float)(icy - 24) }, { (float)(icx - 4), (float)(icy - 8) }, 2.0f, Fade(TEAL, 0.70f));
        DrawLineEx({ (float)(icx - 4), (float)(icy - 8) }, { (float)(icx - 14),(float)(icy + 8) }, 2.0f, Fade(TEAL, 0.70f));
        DrawCircleLines(icx + 14, icy - 12, 13, Fade(TEAL, 0.65f));
        DrawLineEx({ (float)(icx + 14),(float)(icy + 1) }, { (float)(icx + 14),(float)(icy + 24) }, 2.5f, Fade(TEAL, 0.80f));

        int ty = cardY + 148;
        const char* title = "DINE IN";
        DrawText(title, cx + cardW / 2 - MeasureText(title, 24) / 2, ty, 24, TEAL);
        DrawLine(cx + 60, ty + 30, cx + cardW - 60, ty + 30, Fade(TEAL, 0.25f));
        DrawText("Reserve a table & enjoy fine dining",
            cx + cardW / 2 - MeasureText("Reserve a table & enjoy fine dining", 13) / 2,
            ty + 40, 13, SOFT_WHT);

        const char* feats[4] = {
            "Walk-in or Advance booking","Standard / Business / Premium","View & cancel reservations","Real-time availability"
        };
        int fy = ty + 80;
        for (int i = 0; i < 4; i++) {
            float pulse = 0.55f + 0.35f * sinf(time * 2.0f + i * 0.7f);
            DrawRectangle(cx + 30, fy + i * 28 + 7, 6, 6, Fade(TEAL, pulse));
            DrawText(feats[i], cx + 46, fy + i * 28, 13, MUTED);
        }

        int btnY = cardY + cardH - 64;
        bool bHov = MouseOver(cx + 20, btnY, cardW - 40, 48);
        DrawRectangle(cx + 20, btnY, cardW - 40, 48, bHov ? TEAL : Fade(TEAL, 0.10f));
        DrawRectangleLines(cx + 20, btnY, cardW - 40, 48, Fade(TEAL, bHov ? 1.0f : 0.38f));
        const char* blbl = bHov ? "Enter Hall  ->" : "Select  ->";
        DrawText(blbl, cx + cardW / 2 - MeasureText(blbl, 15) / 2, btnY + 17, 15,
            bHov ? PANEL : Fade(TEAL, 0.65f));
        if (bHov && Clicked()) { currentScreen = 0; return; }
    }

    //Card-2: Special Event
    {
        int cx = card2x;

        bool hov = MouseOver(cx, cardY, cardW, cardH);

        // Background
        DrawRectangle(cx, cardY, cardW, cardH, Fade(PANEL, hov ? 0.97f : 0.90f));
        DrawRectangleLines(cx, cardY, cardW, cardH, Fade(RED_C, hov ? 0.85f : 0.30f));

        // Top border
        DrawRectangle(cx, cardY, cardW, 5, RED_C);

        // Center icon
        int sx = cx + cardW / 2;
        int sy = cardY + 90;

        DrawCircle(sx, sy, 40, Fade(RED_C, 0.08f));
        DrawCircleLines(sx, sy, 40, Fade(RED_C, 0.35f));

        // Title
        int ty = cardY + 148;
        const char* title = "SPECIAL EVENTS";

        DrawText(title,
            cx + cardW / 2 - MeasureText(title, 22) / 2,
            ty, 22, RED_C);

        // Subtitle
        DrawText("Celebrate your moments in style",
            cx + cardW / 2 - MeasureText("Celebrate your moments in style", 13) / 2,
            ty + 38, 13, SOFT_WHT);

        // Features
        const char* feats[4] = {
            "Birthday & Anniversary",
            "Corporate meetings",
            "DJ / Decor / Special Menu",
            "4 premium packages"
        };

        int fy = ty + 80;

        for (int i = 0; i < 4; i++) {
            DrawText(feats[i], cx + 46, fy + i * 28, 13, MUTED);
        }

        // Packages
        const char* pkgs[4] = { "Basic","Business","Premium","Corporate" };

        int chipY = cardY + cardH - 120;
        int chipW4 = (cardW - 40 - 12) / 4;

        for (int i = 0; i < 4; i++) {
            int px = cx + 20 + i * (chipW4 + 4);

            DrawRectangle(px, chipY, chipW4, 24, Fade(SOFT_WHT, 0.1f));
            DrawRectangleLines(px, chipY, chipW4, 24, Fade(RED_C, 0.4f));

            DrawText(pkgs[i],
                px + (chipW4 - MeasureText(pkgs[i], 9)) / 2,
                chipY + 8, 9, RED_C);
        }

        // Button
        int btnY = cardY + cardH - 64;
        bool bHov = MouseOver(cx + 20, btnY, cardW - 40, 48);
        DrawRectangle(cx + 20, btnY, cardW - 40, 48,
            bHov ? RED_C : Fade(RED_C, 0.10f));

        const char* blbl = bHov ? "Book Event ->" : "Select ->";
        DrawText(blbl,
            cx + cardW / 2 - MeasureText(blbl, 15) / 2,
            btnY + 17, 15,
            bHov ? PANEL : RED_C);
        if (bHov && Clicked()) {
            currentScreen = 2;
            return;
        }
    }

    // Bottom Find Booking Bar
    int fbY = cardY + cardH + 8;

    // Background
    DrawRectangle(margin, fbY, totalW, findBarH, Fade(PANEL, 0.82f));
    DrawRectangleLines(margin, fbY, totalW, findBarH, Fade(GOLD, 0.28f));

    // Label text
    const char* fbLabel = "Already booked? Look up your Reservation or Event Booking by ID";
    DrawText(fbLabel, margin + 50, fbY + findBarH / 2 - 8, 13, MUTED);

    // Button
    int findBtnW = 210;
    int fbBtnX = margin + totalW - findBtnW - 10;

    bool fbHov = MouseOver(fbBtnX, fbY + 8, findBtnW, findBarH - 16);

    // Button design
    DrawRectangle(fbBtnX, fbY + 8, findBtnW, findBarH - 16,
        fbHov ? GOLD : Fade(GOLD, 0.12f));

    DrawRectangleLines(fbBtnX, fbY + 8, findBtnW, findBarH - 16,
        Fade(GOLD, fbHov ? 1.0f : 0.40f));

    // Button text
    const char* fbBtn = "Find My Booking ->";

    DrawText(fbBtn,
        fbBtnX + (findBtnW - MeasureText(fbBtn, 13)) / 2,
        fbY + findBarH / 2 - 7,
        13,
        fbHov ? PANEL : Fade(GOLD, 0.80f));

    // Button click action
    if (fbHov && Clicked()) {

        // reset search input
        for (int i = 0; i < sizeof(search_input); i++) {
            search_input[i] = '\0';
        }

        search_input_on = false;
        search_input_tick = 0;

        search_resIDX = -2;
        search_evtIDX = -2;
        // reset error
        for (int i = 0; i < sizeof(search_error); i++) {
            search_error[i] = '\0';
        }
        search_tab = 0;
        search_cancelConfirmRes = -1;
        search_cancelConfirmEvt = -1;

        currentScreen = 7;
        return;
    }
    
}
//Screen-0 Floor plan
void ReservationManager::drawFloorPlan(){
    DrawBG();
    TopBar("Table Reservation  |  Select a Table", true);
    if (BackClicked()) { currentScreen = 5; selectedTable = -1; partySize = 2; return; }

    int sw = GetScreenWidth(), sh = GetScreenHeight();

    // Sidebar
    DrawRectangle(0, 56, 240, sh - 56, PANEL);
    DrawRectangleLines(0, 56, 240, sh - 56, Fade(GOLD, 0.3f));
    Heading("PARTY SIZE", 16, 72, 12);
    DrawText("How many guests?", 16, 94, 12, MUTED);
    if (Button(16, 112, 36, 36, "-", Fade(GOLD, 0.6f), 20) && partySize > 1)  partySize--;
    DrawRectangle(58, 112, 60, 36, Fade(BG, 0.8f));
    DrawRectangleLines(58, 112, 60, 36, GOLD);
    char pStr[8]; sprintf(pStr, "%d", partySize);
    DrawText(pStr, 58 + (60 - MeasureText(pStr, 20)) / 2, 120, 20, GOLD);
    if (Button(124, 112, 36, 36, "+", Fade(GOLD, 0.6f), 20) && partySize < 20) partySize++;

    DrawLine(12, 162, 228, 162, Fade(GOLD, 0.3f));
    Heading("LEGEND", 16, 170, 12);
    Color legC[3] = { GREEN, RED_C, MUTED };
    const char* legL[3] = { "Available","Occupied","Too Small" };
    for (int i = 0; i < 3; i++) {
        DrawRectangle(16, 188 + i * 28, 14, 14, legC[i]);
        DrawText(legL[i], 36, 188 + i * 28, 12, SOFT_WHT);
    }

    if (selectedTable >= 0 && selectedTable < 12) {
        DrawLine(12, 278, 228, 278, Fade(GOLD, 0.3f));
        Heading("SELECTED", 16, 286, 12);
        DrawRectangle(12, 306, 218, 54, Fade(GOLD, 0.10f));
        DrawRectangleLines(12, 306, 218, 54, Fade(GOLD, 0.5f));
        DrawText(table_name[selectedTable], 20, 312, 22, SOFT_WHT);
        DrawText(table_type[selectedTable], 20, 338, 12, GOLD);
        char capStr[32]; sprintf(capStr, "Seats %d", table_capacity[selectedTable]);
        DrawText(capStr, 120, 338, 12, MUTED);
    }
    // Floor map
    int mx = 248, my = 60, mw = sw - 256, mh = sh - 130;
    DrawRectangle(mx, my, mw, mh, Fade(PANEL, 0.5f));
    DrawRectangleLines(mx, my, mw, mh, Fade(GOLD, 0.25f));
    const char* hallLbl = "DINE 360  -  MAIN DINING HALL";
    DrawText(hallLbl, mx + mw / 2 - MeasureText(hallLbl, 11) / 2, my + 6, 11, Fade(GOLD, 0.4f));

    struct ZoneInfo { const char* label; Color col; int start, count; };
    ZoneInfo zones[4] = {
        {"WINDOW LOUNGE",TEAL, 0, 3},
        {"INDOOR HALL",Fade(SOFT_WHT,0.7f),3, 4},
        {"OUTDOOR TERRACE", GREEN, 7, 3},
        {"PRIVATE ROOMS",GOLD,10, 2}
    };
    int zoneX[4] = { mx + 8, mx + 8, mx + 8, mx + (mw - 16) * 3 / 5 + 8 };
    int zoneY[4] = { my + 22, my + 172, my + 322, my + 322 };
    int zoneW[4] = { mw - 16, mw - 16, (mw - 16) * 3 / 5 - 4, (mw - 16) * 2 / 5 - 4 };
    int zoneH = 140;
    float t = (float)GetTime();

    for (int z = 0; z < 4; z++) {
        int zx = zoneX[z], zy = zoneY[z], zw = zoneW[z];
        DrawRectangle(zx, zy, zw, zoneH, Fade(zones[z].col, 0.05f));
        DrawRectangleLines(zx, zy, zw, zoneH, Fade(zones[z].col, 0.25f));
        DrawRectangle(zx, zy, zw, 18, Fade(zones[z].col, 0.15f));
        DrawText(zones[z].label, zx + 6, zy + 3, 11, Fade(zones[z].col, 0.85f));

        int cnt = zones[z].count, start = zones[z].start;
        int cw = (zw - (cnt + 1) * 8) / cnt, ch = zoneH - 30;

        for (int j = 0; j < cnt; j++) {
            int ti = start + j;
            int cx2 = zx + 8 + j * (cw + 8), cy2 = zy + 22;
            bool occ = tableOccupied[ti] == 1;
            bool small2 = !occ && partySize > table_capacity[ti];
            bool sel = (selectedTable == ti);
            bool hov = MouseOver(cx2, cy2, cw, ch);
            Color cardBg = sel ? Fade(GOLD, 0.22f) : occ ? Fade(RED_C, 0.15f) : small2 ? Fade(MUTED, 0.12f) : Fade(zones[z].col, 0.08f + 0.03f * sinf(t * 1.5f + ti));
            Color cardBrd = sel ? GOLD : occ ? RED_C : small2 ? MUTED : hov ? zones[z].col : Fade(zones[z].col, 0.5f);
            DrawRectangle(cx2, cy2, cw, ch, cardBg);
            DrawRectangleLines(cx2, cy2, cw, ch, cardBrd);
            if (sel) DrawRectangle(cx2, cy2, cw, 3, GOLD);
            DrawCircleLines(cx2 + cw / 2, cy2 + ch / 2 - 4, (float)(ch / 2 - 16), Fade(cardBrd, 0.5f));
            DrawText(table_name[ti], cx2 + 4, cy2 + 4, 13, sel ? GOLD : SOFT_WHT);
            const char* stxt = occ ? "BUSY" : small2 ? "SMALL" : sel ? "PICKED" : "FREE";
            Color sclr = occ ? RED_C : small2 ? MUTED : sel ? GOLD : GREEN;
            DrawText(stxt, cx2 + 4, cy2 + ch - 16, 10, sclr);
            if (!occ && !small2 && hov && Clicked()) selectedTable = ti;
        }
    }
    // Bottom action bar
    int barX = 248, barW = sw - 256, barH = 66;
    DrawRectangle(barX, sh - barH, barW, barH, PANEL);
    DrawRectangleLines(barX, sh - barH, barW, barH, Fade(GOLD, 0.3f));
    DrawRectangle(barX, sh - barH, barW, 2, GOLD);

    if (selectedTable < 0) {
        const char* p = "Click an available table to continue";
        DrawText(p, barX + barW / 2 - MeasureText(p, 14) / 2, sh - barH / 2 - 7, 14, MUTED);
    }
    else {
        int btnW2 = 240, gap2 = 20;
        int totalBW = btnW2 * 2 + gap2;
        int btn1X = barX + barW / 2 - totalBW / 2;
        int btn2X = btn1X + btnW2 + gap2;
        int btnY2 = sh - barH / 2 - 20;

        if (Button(btn1X, btnY2, btnW2, 40, "Sit Now  (Walk-in)", GREEN)) {
            currentScreen = 6;
        }  
        if (Button(btn2X, btnY2, btnW2, 40, "Book for Later", GOLD)) {

            // Copy selected table type
            if (selectedTable >= 0 && selectedTable < 12) {
                strcpy(booking_tableType, table_type[selectedTable]);
            }

            // Clear date
            for (int i = 0; i < sizeof(booking_date); i++) {
                booking_date[i] = '\0';
            }

            // Clear time
            for (int i = 0; i < sizeof(booking_time); i++) {
                booking_time[i] = '\0';
            }

            // Clear note
            for (int i = 0; i < sizeof(booking_note); i++) {
                booking_note[i] = '\0';
            }

            // Clear error
            for (int i = 0; i < sizeof(booking_error); i++) {
                booking_error[i] = '\0';
            }

            // Reset input states
            booking_date_on = false;
            booking_time_on = false;
            booking_note_on = false;

            booking_date_tick = 0;
            booking_time_tick = 0;
            booking_note_tick = 0;

            booking_pkg = 0;

            // Go to next screen
            currentScreen = 1;
        }
    }
}

//Screen 6 — Walk in Seating
void ReservationManager::drawWalkIn()
{
    DrawBG();
    TopBar("Walk-In Seating", true);
    // Back button
    if (BackClicked()) {
        currentScreen = 0;
        return;
    }
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    // Panel position
    int panW = 500;
    int panH = 380;
    int panX = screen_width / 2 - panW / 2;
    int panY = screen_height / 2 - panH / 2 - 10;
    // Panel box
    DrawRectangle(panX, panY, panW, panH, Fade(PANEL, 0.97f));
    DrawRectangleLines(panX, panY, panW, panH, GREEN);
    // Title
    DrawText("WALK-IN SEATING",panX + panW / 2 - MeasureText("WALK-IN SEATING", 20) / 2,panY + 20, 20, GREEN);

    // Show table info
    if (selectedTable >= 0 && selectedTable < 12) {

        DrawText(table_name[selectedTable],
            panX + panW / 2 - MeasureText(table_name[selectedTable], 40) / 2,
            panY + 60, 40, SOFT_WHT);

        char info[80];
        sprintf(info, "%s Table - Capacity: %d guests",
            table_type[selectedTable],
            table_capacity[selectedTable]);

        DrawText(info,
            panX + panW / 2 - MeasureText(info, 14) / 2,
            panY + 110, 14, MUTED);
    }

    // Question
    DrawText("How many guests?",panX + panW / 2 - MeasureText("How many guests?", 16) / 2,panY + 150, 16, SOFT_WHT);

    int start_Y = panY + 180;
    int start_centerX = panX + panW / 2;

    // Minus button
    if (Button(start_centerX - 100, start_Y, 50, 50, "-", GREEN, 24)) {
        if (partySize > 1) partySize--;
    }
    // Show number
    DrawRectangle(start_centerX - 40, start_Y, 80, 50, Fade(BG, 0.85f));
    DrawRectangleLines(start_centerX - 40, start_Y, 80, 50, GREEN);

    char pStr[8];
    sprintf(pStr, "%d", partySize);

    DrawText(pStr,
        start_centerX - MeasureText(pStr, 28) / 2,
        start_Y + 10, 28, GREEN);

    // Plus button
    if (Button(start_centerX + 50, start_Y, 50, 50, "+", GREEN, 24)) {
        if (partySize < 20) partySize++;
    }

    // Capacity check
    bool overCap = false;
    if (selectedTable >= 0 && selectedTable < 12) {
        if (partySize > table_capacity[selectedTable]) {
            overCap = true;

            char warn[60];
            sprintf(warn, "Max capacity: %d", table_capacity[selectedTable]);

            DrawText(warn,
                panX + panW / 2 - MeasureText(warn, 14) / 2,
                start_Y + 60, 14, RED_C);
        }
    }
    // Date & time
    char date[32];
    char time2[32];

    getTodayStr(date);
    getNowTimeStr(time2);

    char stamp[80];
    sprintf(stamp, "Date: %s   Time: %s", date, time2);

    DrawText(stamp,
        panX + panW / 2 - MeasureText(stamp, 13) / 2,
        panY + 260, 13, TEAL);

    // Confirm button
    if (Button(panX + 30, panY + panH - 70, panW - 60, 50,
        "Confirm Seating",
        overCap ? MUTED : GREEN) && !overCap)
    {
        if (selectedTable >= 0 && selectedTable < 12) {
            addReservation(selectedTable,
                table_type[selectedTable],
                date, time2,
                partySize, 0);
        }

        selectedTable = -1;
        currentScreen = 3;
    }
}

//Screen-1 Advance Booking Form
void ReservationManager::drawBookForm()
{
    DrawBG();
    TopBar("Standard Table Booking", true);
    if (BackClicked()) { currentScreen = 0; return; }

    int screen_width = GetScreenWidth();
    int screen_hight = GetScreenHeight();
    int splitX = (int)(screen_width * 0.57f);
    int fLeft = 36, fRight = splitX - 20, fWidth = fRight - fLeft;

    DrawRectangle(16, 66, splitX - 24, screen_hight - 80, Fade(PANEL, 0.93f));
    DrawRectangleLines(16, 66, splitX - 24, screen_hight - 80, Fade(GOLD, 0.30f));
    DrawRectangle(16, 66, splitX - 24, 3, GOLD);
    Heading("BOOKING DETAILS", fLeft, 82, 13);

    //Selected Table Info
    if (selectedTable >= 0 && selectedTable < 12) {
        DrawRectangle(fLeft, 106, fWidth, 44, Fade(TEAL, 0.10f));
        DrawRectangleLines(fLeft, 106, fWidth, 44, Fade(TEAL, 0.40f));
        DrawText("SELECTED TABLE", fLeft + 8, 110, 10, Fade(TEAL, 0.80f));
        DrawText(table_name[selectedTable], fLeft + 8, 124, 18, SOFT_WHT);

        std::string desc = std::string(table_type[selectedTable]) + " - " +
            std::to_string(table_capacity[selectedTable]) + " guests max";
        DrawText(desc.c_str(), fLeft + 80, 128, 12, MUTED);
    }

    int halfW = (fWidth - 8) / 2;

    DrawText("Date", fLeft, 164, 13, MUTED);
    InputBox(booking_date, &booking_date_on, &booking_date_tick, fLeft, 182, halfW, 44, "DD/MM/YYYY");

    DrawText("Time Slot", fLeft + halfW + 8, 164, 13, MUTED);
    InputBox(booking_time, &booking_time_on, &booking_time_tick, fLeft + halfW + 8, 182, halfW, 44, "e.g. 07:30 PM");

    //Quick time buttons
    const char* qt[5] = { "12:00 PM","01:30 PM","07:00 PM","08:30 PM","09:00 PM" };
    int qBtnW = (fWidth - 16) / 5;

    for (int i = 0; i < 5; i++) {
        int bx = fLeft + i * (qBtnW + 4);
        bool sel = (strcmp(booking_time, qt[i]) == 0);

        DrawRectangle(bx, 234, qBtnW, 28, sel ? Fade(GOLD, 0.22f) : Fade(BG, 0.6f));
        DrawRectangleLines(bx, 234, qBtnW, 28, sel ? GOLD : Fade(GOLD, 0.25f));
        DrawText(qt[i], bx + (qBtnW - MeasureText(qt[i], 11)) / 2, 241, 11, sel ? GOLD : MUTED);

        if (MouseOver(bx, 234, qBtnW, 28) && Clicked())
            strcpy(booking_time, qt[i]);
    }

    DrawText("Party Size", fLeft, 278, 13, MUTED);

    std::string partyTxt = std::to_string(partySize) + " guests  (set on floor plan)";
    DrawText(partyTxt.c_str(), fLeft, 296, 14, SOFT_WHT);

    DrawText("Special Note (optional)", fLeft, 328, 13, MUTED);
    InputBox(booking_note, &booking_note_on, &booking_note_tick,
        fLeft, 346, fWidth, 44, "Any dietary needs, allergies, occasions...");

    //Packages
    Heading("DINING PACKAGE", fLeft, 410, 13);
    DrawText("Choose your experience level", fLeft, 428, 12, MUTED);

    const char* package_name[3] = { "STANDARD","BUSINESS","PREMIUM" };
    const char* package_line1[3] = {
        "Table only - order ","Starter + mineral water + WiFi","3-course meal + dedicated waiter"
    };
    const char* package_line2[3] = {
        "No extras included","Priority seating guaranteed","Complimentary dessert tray"
    };
    int package_cost[3] = { 0, 800, 2000 };
    Color package_color[3] = { SOFT_WHT, TEAL, GOLD };
    int package_width = (fWidth - 8) / 3;

    for (int i = 0; i < 3; i++) {
        int px = fLeft + i * (package_width + 4);
        bool sel = (booking_pkg == i);
        bool hov = MouseOver(px, 448, package_width, 110);

        DrawRectangle(px, 448, package_width, 110,sel ? Fade(package_color[i], 0.18f) :hov ? Fade(package_color[i], 0.09f) : Fade(BG, 0.6f));

        DrawRectangleLines(px, 448, package_width, 110,sel ? package_color[i] : Fade(package_color[i], 0.35f));

        DrawText(package_name[i], px + 10, 460, 14, sel ? package_color[i] : SOFT_WHT);
        DrawText(package_line1[i], px + 10, 482, 10, MUTED);
        DrawText(package_line2[i], px + 10, 496, 10, MUTED);

        if (package_cost[i] > 0) {
            std::string cost = "+Rs." + std::to_string(package_cost[i]) + "/head";
            DrawText(cost.c_str(), px + 10, 542, 11, package_color[i]);
        }
        else {
            DrawText("Included", px + 10, 542, 11, GREEN);
        }

        if (hov && Clicked()) booking_pkg = i;
    }

    // Error
    if (strlen(booking_error))
        DrawText(booking_error, fLeft, screen_hight - 100, 12, RED_C);

    // Confirm Button
    if (Button(fLeft, screen_hight - 82, fWidth, 54, "Confirm Reservation", GOLD)) {

        memset(booking_error, 0, sizeof(booking_error));

        if (!strlen(booking_date) || !strlen(booking_time)) {
            strcpy(booking_error, "Please enter both Date and Time.");
        }
        else if (selectedTable >= 0 && selectedTable < 12) {
            addReservation(selectedTable, table_type[selectedTable],
                booking_date, booking_time, partySize, booking_pkg);

            selectedTable = -1;
            partySize = 2;
            currentScreen = 3;
        }
    }

    // Receipt Panel
    int rx = splitX + 8, rw = screen_width - splitX - 24;
    DrawRectangle(rx, 66, rw, screen_hight - 80, Fade(PANEL, 0.85f));
    DrawRectangleLines(rx, 66, rw, screen_hight - 80, Fade(GOLD, 0.30f));

    int rl = rx + 18, rr = rx + rw - 18, ry = 130;

    auto row = [&](const char* label, const char* val) {
        DrawText(label, rl, ry, 13, MUTED);
        DrawText(val, rr - MeasureText(val, 13), ry, 13, SOFT_WHT);
        ry += 28;
        };

    row("Table", (selectedTable >= 0 && selectedTable < 12) ? table_name[selectedTable] : "--");
    row("Date", strlen(booking_date) ? booking_date : "--");
    row("Time", strlen(booking_time) ? booking_time : "--");

    std::string partyStr = std::to_string(partySize) + " guests";
    row("Party", partyStr.c_str());

    const char* pNames[3] = { "Standard","Business","Premium" };
    row("Package", pNames[booking_pkg]);

    int total = package_cost[booking_pkg] * partySize;
    std::string totStr = "Rs." + std::to_string(total);

    DrawText(totStr.c_str(), rr - MeasureText(totStr.c_str(), 26), ry, 26, GOLD);
}
//Screen-2 Event Form
void ReservationManager::drawEventForm()
{
    DrawBG();
    TopBar("Book a Special Event", true);

    // Back button
    if (BackClicked()) {
        event_date[0] = '\0';
        event_time[0] = '\0';
        event_guests[0] = '\0';
        event_error[0] = '\0';

        ev_date_on = ev_time_on = ev_guests_on = false;
        ev_type = 0;
        ev_pkg = 0;
        ev_deco = ev_menu = event_sound = false;

        currentScreen = 5;
        return;
    }
    int screen_width = GetScreenWidth();

    DrawRectangle(16, 66, 700, 670, PANEL);
    DrawRectangleLines(16, 66, 700, 670, Fade(TEAL, 0.3f));
    DrawRectangle(16, 66, 700, 3, TEAL);

    Heading("EVENT TYPE", 34, 82, 13);

    Color etClr[3] = { GOLD, Fade(SOFT_WHT,0.7f), TEAL };

    // Event type selection
    for (int i = 0; i < 3; i++) {
        bool selected = (ev_type == i);
        int x = 34 + i * 226;

        DrawRectangle(x, 100, 220, 42,
            selected ? Fade(etClr[i], 0.22f) : Fade(etClr[i], 0.07f));

        DrawRectangleLines(x, 100, 220, 42,
            selected ? etClr[i] : Fade(etClr[i], 0.35f));

        if (selected)
            DrawRectangle(x, 100, 220, 3, etClr[i]);

        DrawText(event_type[i],
            x + (220 - MeasureText(event_type[i], 14)) / 2,
            114, 14,
            selected ? etClr[i] : MUTED);

        if (MouseOver(x, 100, 220, 42) && Clicked())
            ev_type = i;
    }

    // Input fields
    DrawText("Date", 34, 156, 13, MUTED);
    InputBox(event_date, &ev_date_on, &ev_date_tick, 34, 174, 310, 44, "DD/MM/YYYY");

    DrawText("Time", 356, 156, 13, MUTED);
    InputBox(event_time, &ev_time_on, &ev_time_tick, 356, 174, 340, 44, "07:00 PM");

    DrawText("Number of Guests", 34, 230, 13, MUTED);
    InputBox(event_guests, &ev_guests_on, &ev_guests_tick, 34, 248, 180, 44, "30");

    // Packages
    Heading("EVENT PACKAGE", 34, 308, 13);

    Color event_package_color[4] = { SOFT_WHT, TEAL, GOLD, Fade(RED_C,0.8f) };

    for (int i = 0; i < 4; i++) {
        int x = 34 + i * 168;
        bool selected = (ev_pkg == i);

        DrawRectangle(x, 328, 162, 100,selected ? Fade(event_package_color[i], 0.20f) : Fade(PANEL, 0.5f));

        DrawRectangleLines(x, 328, 162, 100,selected ? event_package_color[i] : Fade(event_package_color[i], 0.4f));

        if (selected) {
            DrawRectangle(x, 328, 162, 3, event_package_color[i]);
        }

        DrawText(event_package[i], x + 10, 338, 14,selected ? event_package_color[i] : SOFT_WHT);

        std::string feeText = "Rs." + std::to_string(event_fee[i]) + " flat";
        DrawText(feeText.c_str(), x + 10, 356, 11, MUTED);

        if (MouseOver(x, 328, 162, 100) && Clicked())
            ev_pkg = i;
    }
    // Add-ons
    Heading("ADD-ONS", 34, 442, 12);

    ev_deco = Toggle(34, 468, 170, 40, "Decorations", ev_deco, GOLD);
    ev_menu = Toggle(216, 468, 170, 40, "Special Menu", ev_menu, TEAL);
    event_sound = Toggle(398, 468, 170, 40, "DJ / Sound", event_sound, RED_C);

    // Error message
    if (strlen(event_error))
        DrawText(event_error, 34, 524, 13, RED_C);

    // Confirm button
    if (Button(34, 542, 664, 48, "Confirm Event Booking", TEAL)) {

        event_error[0] = '\0';

        if (!strlen(event_date) || !strlen(event_time) || !strlen(event_guests)) {
            strcpy(event_error, "All fields are required");
        }
        else {
            int guests = atoi(event_guests);

            if (guests < 1) {
                strcpy(event_error, "Guests must be at least 1");
            }
            else {
                addEvent(event_type[ev_type], event_date, event_time,
                    guests, ev_pkg, ev_deco, ev_menu, event_sound);

                // Reset
                event_date[0] = '\0';
                event_time[0] = '\0';
                event_guests[0] = '\0';

                ev_deco = ev_menu = event_sound = false;
                event_error[0] = '\0';

                currentScreen = 4;
            }
        }
    }

    // ================= Right Panal =================

    DrawRectangle(724, 66, screen_width - 740, 670, Fade(PANEL, 0.8f));
    DrawRectangleLines(724, 66, screen_width - 740, 670, Fade(TEAL, 0.25f));

    Heading("EVENT PREVIEW", 742, 82, 13);

    DrawText(event_type[ev_type], 742, 126, 18, SOFT_WHT);

    DrawText(strlen(event_date) ? event_date : "--", 742, 178, 16, SOFT_WHT);
    DrawText(strlen(event_time) ? event_time : "--", 742, 224, 16, SOFT_WHT);

    //Guests text 
    std::string guestText = std::string(strlen(event_guests) ? event_guests : "0") + " guests";

    DrawText(guestText.c_str(), 742, 270, 18, SOFT_WHT);

    int g = atoi(event_guests);

    float total =
        event_fee[ev_pkg] + (500 * g) + (ev_deco ? 5000 : 0) + (ev_menu ? 3000 : 0) + (event_sound ? 8000 : 0);

    std::string totalText = "Rs." + std::to_string((int)total);

    DrawText(totalText.c_str(), 742, 340, 26, GOLD);
}
//Screen-3 View Reservation
void ReservationManager::drawResList(){
    DrawBG();
    TopBar("All Reservations", true);
    //Back button
    if (BackClicked()) {
        resScroll = 0;
        cancelConfirmRes = -1;
        currentScreen = 5;
        return;
    }
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    //Find by ID button
    if (OutlineBtn(screenWidth - 200, 62, 188, 28, "Find by ID  ->", GOLD, 11)) {
        search_input[0] = '\0';
        search_error[0] = '\0';

        search_input_on = false;
        search_input_tick = 0;

        search_resIDX = -2;
        search_evtIDX = -2;

        search_tab = 0;
        search_cancelConfirmRes  = -1;
        search_cancelConfirmEvt = -1;

        currentScreen = 7;
        return;
    }

    //Top info bar
    DrawRectangle(0, 56, screenWidth, 38, PANEL);

    int activeCount = 0;
    for (int i = 0; i < resCount; i++) {
        if (strcmp(reservations[i].getStatus(), "Confirmed") == 0)
            activeCount++;
    }
    std::string infoText ="Total: " + std::to_string(resCount) + " | Active: " + std::to_string(activeCount) + " | Tables: " + std::to_string(getOccupied()) + " / 12";

    DrawText(infoText.c_str(), 20, 65, 13, SOFT_WHT);
    //No reservations
    if (resCount == 0) {
        const char* msg = "No reservations yet. Make your first booking!";
        DrawText(msg,
            screenWidth / 2 - MeasureText(msg, 16) / 2, screenHeight / 2, 16, MUTED);
        return;
    }
    //Table header
    int headerY = 100;

    DrawRectangle(0, headerY, screenWidth, 26, Fade(PANEL, 0.9f));

    DrawText("ID", 20, headerY + 6, 12, MUTED);
    DrawText("TABLE", 80, headerY + 6, 12, MUTED);
    DrawText("TYPE", 160, headerY + 6, 12, MUTED);
    DrawText("DATE", 290, headerY + 6, 12, MUTED);
    DrawText("TIME", 430, headerY + 6, 12, MUTED);
    DrawText("GUESTS", 570, headerY + 6, 12, MUTED);
    DrawText("STATUS", screenWidth - 280, headerY + 6, 12, MUTED);
    DrawText("ACTION", screenWidth - 158, headerY + 6, 12, MUTED);
    //List rows
    int rowHeight = 54;
    int y = headerY + 28 - resScroll;

    for (int i = 0; i < resCount; i++) {

        if (y > screenHeight) {
            break;
        }
        if (y < headerY) {
            y += rowHeight + 4;
            continue;
        }
        bool isCancelled = (strcmp(reservations[i].getStatus(), "Cancelled") == 0);
        bool isConfirmingCancel = (cancelConfirmRes == i);
        //Row background
        Color rowColor;
        if (isCancelled) {
            rowColor = Fade(PANEL, 0.28f);
        }
        else if (i % 2 == 0) {
            rowColor = Fade(PANEL, 0.60f);
        }
        else {
            rowColor = Fade(PANEL, 0.40f);
        }
        DrawRectangle(0, y, screenWidth, rowHeight, rowColor);
        DrawRectangleLines(0, y, screenWidth, rowHeight, Fade(GOLD, 0.12f));
        // Left color bar
        DrawRectangle(0, y, 4, rowHeight, isCancelled ? RED_C : GOLD);
        //Cancelled line
        if (isCancelled) {
            DrawLine(8, y + rowHeight / 2,
                screenWidth - 8,
                y + rowHeight / 2,
                Fade(RED_C, 0.20f));
        }
        Color textColor = isCancelled ? MUTED : SOFT_WHT;
        //ID
        std::string idText = "#" + std::to_string(reservations[i].getID());
        DrawText(idText.c_str(), 20, y + 18, 14, isCancelled ? MUTED : GOLD);
        //Table
        int tableIndex = reservation_tableIDX[i];
        const char* tableName = (tableIndex >= 0 && tableIndex < 12) ? table_name[tableIndex] : "--";
        DrawText(tableName, 80, y + 18, 13, isCancelled ? MUTED : TEAL);
        //Details
        DrawText(reservations[i].getTableType(), 160, y + 18, 13, textColor);
        DrawText(reservations[i].getDate(), 290, y + 18, 13, textColor);
        DrawText(reservations[i].getTime(), 430, y + 18, 13, textColor);
        //Guests
        std::string guestText = std::to_string(reservations[i].getPartySize()) + " pax";
        DrawText(guestText.c_str(), 570, y + 18, 13, textColor);
        //Status
        int btnY = y + 14;
        Color statusColor = isCancelled ? RED_C : GREEN;
        DrawRectangle(screenWidth - 290, btnY, 126, 26, Fade(statusColor, 0.14f));
        DrawRectangleLines(screenWidth - 290, btnY, 126, 26, Fade(statusColor, 0.32f));

        const char* statusText = reservations[i].getStatus();

        DrawText(statusText,screenWidth - 290 + (126 - MeasureText(statusText, 12)) / 2,btnY + 7,12,statusColor);
        //Cancel button logic
        if (!isCancelled) {
            if (!isConfirmingCancel) {
                if (OutlineBtn(screenWidth - 154, btnY, 140, 26,
                    "Cancel Booking", RED_C, 11)) {
                    cancelConfirmRes = i;
                }

            }
            else {
                DrawText("Sure?", screenWidth - 160, btnY + 7, 11, RED_C);
                if (Button(screenWidth - 120, btnY, 54, 26, "Yes", RED_C, 12)) {
                    cancelReservation(i);
                }
                if (OutlineBtn(screenWidth - 60, btnY, 54, 26, "Keep", TEAL, 12)) {
                    cancelConfirmRes = -1;
                }
            }

        }
        else {
            DrawText("--", screenWidth - 100, btnY + 7, 12, Fade(MUTED, 0.45f));
        }

        y += rowHeight + 4;
    }
    //Scroll
    resScroll -= (int)(GetMouseWheelMove() * 28);

    if (resScroll < 0) {
        resScroll = 0;
    }
    int maxScroll = resCount * (rowHeight + 4) - (screenHeight - headerY - 28);

    if (maxScroll < 0) {
        maxScroll = 0;
    }
    if (resScroll > maxScroll) {
        resScroll = maxScroll;
    }   
}
//Screen-4 View Events
void ReservationManager::drawEvtList(){
    DrawBG();
    TopBar("All Event Bookings", true);
    //Back button
    if (BackClicked()) {
        evtScroll = 0;
        cancelConfirmEvt = -1;
        currentScreen = 5;
        return;
    }
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    //Find by ID
    if (OutlineBtn(screenWidth - 200, 62, 188, 28, "Find by ID  ->", TEAL, 11)) {
        search_input[0] = '\0';
        search_error[0] = '\0';
        search_input_on = false;
        search_input_tick = 0;
        search_resIDX = -2;
        search_evtIDX = -2;
        search_tab = 1;
        search_cancelConfirmRes = -1;
        search_cancelConfirmEvt = -1;
        currentScreen = 7;
        return;
    }
    //Top info
    DrawRectangle(0, 56, screenWidth, 38, PANEL);
    int activeEvents = 0;
    for (int i = 0; i < evtCount; i++) {
        if (strcmp(events[i].getStatus(), "Confirmed") == 0)
            activeEvents++;
    }
    std::string topText = "Total Events: " + std::to_string(evtCount) + " | Active: " + std::to_string(activeEvents);
    DrawText(topText.c_str(), 20, 65, 13, SOFT_WHT);
    //No events
    if (evtCount == 0) {
        const char* msg = "No events booked yet.";
        DrawText(msg,screenWidth / 2 - MeasureText(msg, 16) / 2,screenHeight / 2,16, MUTED);
        return;
    }
    //Event cards
    int cardHeight = 100;
    int y = 100 - evtScroll;

    for (int i = 0; i < evtCount; i++) {
        if (y > screenHeight) {
            break;
        }
        if (y < 96) {
            y += cardHeight + 10;
            continue;
        }
        bool isCancelled = (strcmp(events[i].getStatus(), "Cancelled") == 0);
        bool confirmCancel = (cancelConfirmEvt == i);
        //Card background
        DrawRectangle(16, y, screenWidth - 32, cardHeight,isCancelled ? Fade(PANEL, 0.28f) : Fade(PANEL, 0.85f));
        DrawRectangleLines(16, y, screenWidth - 32, cardHeight,isCancelled ? Fade(RED_C, 0.30f) : Fade(TEAL, 0.40f));
        DrawRectangle(16, y, 4, cardHeight, isCancelled ? RED_C : TEAL);
        if (isCancelled) {
            DrawLine(22, y + cardHeight / 2,
                screenWidth - 20,
                y + cardHeight / 2,
                Fade(RED_C, 0.22f));
        }
        Color textColor = isCancelled ? MUTED : SOFT_WHT;
        //Event Type
        DrawRectangle(28, y + 10, 120, 22, Fade(TEAL, 0.18f));
        DrawText(events[i].getEventType(), 36, y + 14, 12,isCancelled ? MUTED : TEAL);
        //Package
        int pkgIndex = events[i].getPackage();
        Color pkgColor =isCancelled ? MUTED :(pkgIndex == 2 ? GOLD : (pkgIndex == 1 ? TEAL : SOFT_WHT));
        DrawRectangle(158, y + 10, 100, 22, Fade(pkgColor, 0.14f));
        DrawText(event_package[pkgIndex], 164, y + 14, 12, pkgColor);
        //Event ID
        std::string idText = "Event # " + std::to_string(events[i].getID());
        DrawText(idText.c_str(), 270, y + 12, 16,isCancelled ? MUTED : GOLD);
        //Info (date, time, guests)
        std::string infoText =std::string("Date: ") + events[i].getDate() + "   Time: " + events[i].getTime() + "   Guests: " + std::to_string(events[i].getPartySize());
        DrawText(infoText.c_str(), 28, y + 44, 13, textColor);
        //Total price
        std::string priceText = "Total: Rs. " + std::to_string((int)events[i].getTotalCost());
        DrawText(priceText.c_str(), 28, y + 66, 13, isCancelled ? MUTED : GOLD);
        //Add ons
        int x = 500;
        if (events[i].getDecor()) {
            DrawText("Decor", x, y + 44, 11, MUTED);
            x += 60;
        }
        if (events[i].getSpecialMenu()) {
            DrawText("Menu", x, y + 44, 11, MUTED);
            x += 50;
        }
        if (events[i].getDJ()) {
            DrawText("DJ", x, y + 44, 11, MUTED);
        }
        //Status
        int midY = y + (cardHeight / 2) - 14;
        Color statusColor = isCancelled ? RED_C : GREEN;
        DrawRectangle(screenWidth - 300, midY, 128, 28, Fade(statusColor, 0.14f));
        DrawRectangleLines(screenWidth - 300, midY, 128, 28, Fade(statusColor, 0.35f));
        const char* statusText = events[i].getStatus();
        DrawText(statusText,screenWidth - 300 + (128 - MeasureText(statusText, 12)) / 2,midY + 8,12,statusColor);

        //Cancel logic
        if (!isCancelled) {
            if (!confirmCancel) {
                if (OutlineBtn(screenWidth - 162, midY, 140, 28,"Cancel Event", RED_C, 11)) {cancelConfirmEvt = i;
                }
            }
            else {
                DrawText("Sure?", screenWidth - 162, midY - 12, 10, RED_C);
                if (Button(screenWidth - 162, midY, 62, 28,"Yes", RED_C, 11)) {
                    cancelEvent(i);
                }
                if (OutlineBtn(screenWidth - 94, midY, 72, 28,
                    "Keep", TEAL, 11)) {
                    cancelConfirmEvt = -1;
                }
            }
        }
        else {
            DrawText("--", screenWidth - 130, midY + 8, 12,
                Fade(MUTED, 0.45f));
        }
        y += cardHeight + 10;
    }
    //Scroll system
    evtScroll -= (int)(GetMouseWheelMove() * 28);
    if (evtScroll < 0) {
        evtScroll = 0;
    }
    int maxScroll = evtCount * (cardHeight + 10) - (screenHeight - 100);

    if (maxScroll < 0) {
        maxScroll = 0;
    }  
    if (evtScroll > maxScroll) {
        evtScroll = maxScroll;
    }
       
}
//  Screen-7 — Search by ID
void ReservationManager::drawSearchByID()
{
    DrawBG();
    TopBar("Find My Booking", true);
    if (BackClicked()) {
        search_resIDX = search_evtIDX = -2;
        memset(search_input, 0, sizeof(search_input));
        memset(search_error, 0, sizeof(search_error));
        search_cancelConfirmRes = search_cancelConfirmEvt = -1;
        currentScreen = 5; return;
    }

    int sw = GetScreenWidth(), sh = GetScreenHeight();
    float t = (float)GetTime();

    DrawRectangle(0, 56, sw, 48, Fade(PANEL, 0.95f));
    DrawRectangle(0, 101, sw, 2, Fade(GOLD, 0.30f));

    const char* tabs[2] = { "Reservation","Event Booking" };
    Color tabClr[2] = { TEAL, RED_C };
    int tabW = 200, tabX = sw / 2 - tabW;
    for (int i = 0; i < 2; i++) {
        bool sel = (search_tab == i);
        int tx = tabX + i * (tabW + 4);
        DrawRectangle(tx, 60, tabW, 38, sel ? Fade(tabClr[i], 0.20f) : Fade(PANEL, 0.5f));
        DrawRectangleLines(tx, 60, tabW, 38, sel ? tabClr[i] : Fade(tabClr[i], 0.30f));
        if (sel) DrawRectangle(tx, 96, tabW, 5, tabClr[i]);
        DrawText(tabs[i], tx + (tabW - MeasureText(tabs[i], 14)) / 2, 71, 14, sel ? tabClr[i] : MUTED);
        if (MouseOver(tx, 60, tabW, 38) && Clicked()) {
            search_tab = i; search_resIDX = search_evtIDX = -2;
            memset(search_error, 0, sizeof(search_error));
        }
    }

    Color accentC = tabClr[search_tab];
    int barY = 118;
    DrawRectangle(0, barY, sw, 80, Fade(PANEL, 0.60f));
    DrawRectangle(0, barY + 78, sw, 2, Fade(accentC, 0.25f));
    const char* prompt = search_tab == 0
        ? "Enter Reservation ID  (e.g. 1001)"
        : "Enter Event Booking ID  (e.g. 1002)";
    DrawText(prompt, sw / 2 - MeasureText(prompt, 12) / 2, barY + 8, 12, MUTED);

    int inpW = 380, inpH = 44;
    int inpX = sw / 2 - inpW / 2 - 56, inpY = barY + 26;
    InputBox(search_input, &search_input_on, &search_input_tick, inpX, inpY, inpW, inpH, "Type ID here...");

    bool searchHov = MouseOver(inpX + inpW + 8, inpY, 130, inpH);
    DrawRectangle(inpX + inpW + 8, inpY, 130, inpH, searchHov ? accentC : Fade(accentC, 0.15f));
    DrawRectangleLines(inpX + inpW + 8, inpY, 130, inpH, Fade(accentC, searchHov ? 1.0f : 0.45f));
    DrawText("Search", inpX + inpW + 8 + (130 - MeasureText("Search", 15)) / 2, inpY + 14, 15,
        searchHov ? PANEL : accentC);

    bool doSearch = (searchHov && Clicked()) || (search_input_on && IsKeyPressed(KEY_ENTER));
    if (doSearch) {
        memset(search_error, 0, sizeof(search_error));
        search_resIDX = search_evtIDX = -1;
        int searchID = atoi(search_input);
        if (searchID <= 0) {
            strcpy(search_error, "Please enter a valid numeric ID (e.g. 1001).");
        }
        else if (search_tab == 0) {
            for (int i = 0; i < resCount; i++)
                if (reservations[i].getID() == searchID) { search_resIDX = i; break; }
            if (search_resIDX < 0)
                sprintf(search_error, "No reservation found with ID #%d.", searchID);
        }
        else {
            for (int i = 0; i < evtCount; i++)
                if (events[i].getID() == searchID) { search_evtIDX = i; break; }
            if (search_evtIDX < 0)
                sprintf(search_error, "No event booking found with ID #%d.", searchID);
        }
    }

    if (strlen(search_error)) {
        DrawRectangle(sw / 2 - 260, barY + 74, 520, 22, Fade(RED_C, 0.12f));
        DrawText(search_error, sw / 2 - MeasureText(search_error, 12) / 2, barY + 78, 12, RED_C);
    }

    // Idle state
    if ((search_tab == 0 && search_resIDX == -2) || (search_tab == 1 && search_evtIDX == -2)) {
        int cy = sh / 2 + 20;
        float r = 45.0f + 4.0f * sinf(t * 1.5f);
        DrawCircle(sw / 2, cy, r, Fade(accentC, 0.07f));
        DrawCircleLines(sw / 2, cy, r, Fade(accentC, 0.30f));
        DrawCircleLines(sw / 2, cy, r * 0.55f, Fade(accentC, 0.55f));
        DrawLineEx({ (float)(sw / 2) + (r * 0.38f),(float)cy + (r * 0.38f) },
            { (float)(sw / 2) + (r * 0.75f),(float)cy + (r * 0.75f) }, 3.0f, Fade(accentC, 0.55f));
        const char* hint = search_tab == 0
            ? "Type a Reservation ID above and click Search"
            : "Type an Event Booking ID above and click Search";
        DrawText(hint, sw / 2 - MeasureText(hint, 13) / 2, cy + (int)r + 18, 13, Fade(MUTED, 0.65f));
        char resStat[40], evtStat[40];
        sprintf(resStat, "Reservations in system: %d", resCount);
        sprintf(evtStat, "Event bookings in system: %d", evtCount);
        int chipY = (int)(cy + r + 52);
        DrawRectangle(sw / 2 - 300, chipY, 290, 28, Fade(TEAL, 0.10f));
        DrawRectangleLines(sw / 2 - 300, chipY, 290, 28, Fade(TEAL, 0.25f));
        DrawText(resStat, sw / 2 - 300 + (290 - MeasureText(resStat, 12)) / 2, chipY + 8, 12, Fade(TEAL, 0.75f));
        DrawRectangle(sw / 2 + 10, chipY, 290, 28, Fade(RED_C, 0.10f));
        DrawRectangleLines(sw / 2 + 10, chipY, 290, 28, Fade(RED_C, 0.25f));
        DrawText(evtStat, sw / 2 + 10 + (290 - MeasureText(evtStat, 12)) / 2, chipY + 8, 12, Fade(RED_C, 0.70f));
        return;
    }

    int cardY = 212, cardX = sw / 2 - 480, cardW2 = 960, cardH2 = 0;

    //Reservation result card
    if (search_tab == 0 && search_resIDX >= 0) {
        int ri = search_resIDX;
        bool isCancelled = (strcmp(reservations[ri].getStatus(), "Cancelled") == 0);
        Color statusC = isCancelled ? RED_C : GREEN;
        cardH2 = 340;

        DrawRectangle(cardX + 5, cardY + 5, cardW2, cardH2, Fade(TEAL, 0.05f));
        DrawRectangle(cardX, cardY, cardW2, cardH2, Fade(PANEL, 0.97f));
        DrawRectangleLines(cardX, cardY, cardW2, cardH2, Fade(TEAL, 0.45f));
        DrawRectangle(cardX, cardY, cardW2, 5, TEAL);
        DrawRectangle(cardX, cardY, 5, cardH2, Fade(TEAL, 0.50f));

        DrawRectangle(cardX + cardW2 - 148, cardY + 12, 136, 30, Fade(statusC, 0.15f));
        DrawRectangleLines(cardX + cardW2 - 148, cardY + 12, 136, 30, Fade(statusC, 0.45f));
        DrawText(reservations[ri].getStatus(),
            cardX + cardW2 - 148 + (136 - MeasureText(reservations[ri].getStatus(), 13)) / 2,
            cardY + 20, 13, statusC);

        char titleStr[40]; sprintf(titleStr, "Reservation  # %d", reservations[ri].getID());
        DrawText(titleStr, cardX + 24, cardY + 16, 22, SOFT_WHT);
        DrawLine(cardX + 16, cardY + 52, cardX + cardW2 - 16, cardY + 52, Fade(TEAL, 0.20f));

        int tbl = reservation_tableIDX[ri];
        int col1 = cardX + 24, col2 = cardX + 280, col3 = cardX + 520, col4 = cardX + 720;
        int ry = cardY + 68;

        DrawRectangle(col1, ry, 220, 70, Fade(TEAL, 0.10f));
        DrawRectangleLines(col1, ry, 220, 70, Fade(TEAL, 0.30f));
        DrawText("TABLE", col1 + 10, ry + 8, 10, Fade(TEAL, 0.65f));
        DrawText((tbl >= 0 && tbl < 12) ? table_name[tbl] : "--", col1 + 10, ry + 24, 32, SOFT_WHT);
        if (tbl >= 0 && tbl < 12) DrawText(table_type[tbl], col1 + 10, ry + 56, 11, MUTED);

        auto detRow = [&](int x, int y2, const char* lbl, const char* val, Color valC) {
            DrawText(lbl, x, y2, 11, MUTED);
            DrawText(val, x, y2 + 18, 16, valC);
            };
        detRow(col2, ry, "DATE", reservations[ri].getDate(), SOFT_WHT);
        detRow(col2, ry + 54, "TIME", reservations[ri].getTime(), SOFT_WHT);
        char paxStr[20]; sprintf(paxStr, "%d guests", reservations[ri].getPartySize());
        detRow(col3, ry, "PARTY SIZE", paxStr, SOFT_WHT);

        if (tbl >= 0 && tbl < 12) {
            int cap = table_capacity[tbl], pax = reservations[ri].getPartySize();
            float ratio = (float)pax / cap; if (ratio > 1.0f) ratio = 1.0f;
            DrawText("CAPACITY", col3, ry + 54, 11, MUTED);
            DrawRectangle(col3, ry + 72, 180, 8, Fade(BG, 0.8f));
            DrawRectangle(col3, ry + 72, (int)(180 * ratio), 8, Fade(TEAL, 0.75f));
            DrawRectangleLines(col3, ry + 72, 180, 8, Fade(TEAL, 0.30f));
            char capStr[24]; sprintf(capStr, "%d / %d", pax, cap);
            DrawText(capStr, col3 + 188, ry + 68, 11, MUTED);
        }
        detRow(col4, ry, "STATUS", reservations[ri].getStatus(), statusC);

        if (isCancelled)
            DrawLine(cardX + 16, cardY + cardH2 / 2, cardX + cardW2 - 16, cardY + cardH2 / 2, Fade(RED_C, 0.18f));

        DrawLine(cardX + 16, cardY + 168, cardX + cardW2 - 16, cardY + 168, Fade(TEAL, 0.15f));
        DrawRectangle(cardX + 16, cardY + 178, cardW2 - 32, 40, Fade(BG, 0.6f));
        char sumBuf[160];
        sprintf(sumBuf, "Table %s  |  %s  |  %s  |  %d guests  |  %s",
            (tbl >= 0 && tbl < 12) ? table_name[tbl] : "--",
            (tbl >= 0 && tbl < 12) ? table_type[tbl] : "--",
            reservations[ri].getDate(),
            reservations[ri].getPartySize(),
            reservations[ri].getStatus());
        DrawText(sumBuf, cardX + 24, cardY + 193, 12, MUTED);
        DrawLine(cardX + 16, cardY + 228, cardX + cardW2 - 16, cardY + 228, Fade(TEAL, 0.15f));

        if (!isCancelled) {
            if (search_cancelConfirmRes != ri) {
                if (OutlineBtn(cardX + 24, cardY + 248, 210, 40, "Cancel This Reservation", RED_C, 12))
                    search_cancelConfirmRes = ri;
                DrawText("To keep, simply do nothing.", cardX + 250, cardY + 262, 12, Fade(MUTED, 0.55f));
            }
            else {
                DrawText("Are you sure you want to cancel?", cardX + 24, cardY + 248, 13, RED_C);
                if (Button(cardX + 24, cardY + 270, 108, 36, "Yes, Cancel", RED_C, 12)) cancelReservation(ri);
                if (OutlineBtn(cardX + 148, cardY + 270, 90, 36, "Keep It", TEAL, 12))   search_cancelConfirmRes = -1;
            }
        }
        else {
            DrawRectangle(cardX + 24, cardY + 248, cardW2 - 48, 40, Fade(RED_C, 0.08f));
            DrawText("This reservation has been cancelled. No further action available.",
                cardX + 32, cardY + 264, 12, Fade(RED_C, 0.65f));
        }
    }

    //Event result card
    else if (search_tab == 1 && search_evtIDX >= 0) {
        int ei = search_evtIDX;
        bool isCancelled = (strcmp(events[ei].getStatus(), "Cancelled") == 0);
        Color statusC = isCancelled ? RED_C : GREEN;
        cardH2 = 380;

        DrawRectangle(cardX + 5, cardY + 5, cardW2, cardH2, Fade(RED_C, 0.04f));
        DrawRectangle(cardX, cardY, cardW2, cardH2, Fade(PANEL, 0.97f));
        DrawRectangleLines(cardX, cardY, cardW2, cardH2, Fade(RED_C, 0.45f));
        DrawRectangle(cardX, cardY, cardW2, 5, RED_C);
        DrawRectangle(cardX, cardY, 5, cardH2, Fade(RED_C, 0.50f));

        DrawRectangle(cardX + cardW2 - 148, cardY + 12, 136, 30, Fade(statusC, 0.15f));
        DrawRectangleLines(cardX + cardW2 - 148, cardY + 12, 136, 30, Fade(statusC, 0.45f));
        DrawText(events[ei].getStatus(),
            cardX + cardW2 - 148 + (136 - MeasureText(events[ei].getStatus(), 13)) / 2,
            cardY + 20, 13, statusC);

        char titleStr[40]; sprintf(titleStr, "Event Booking  # %d", events[ei].getID());
        DrawText(titleStr, cardX + 24, cardY + 16, 22, SOFT_WHT);
        DrawLine(cardX + 16, cardY + 52, cardX + cardW2 - 16, cardY + 52, Fade(RED_C, 0.20f));

        Color etClr[3] = { GOLD, Fade(SOFT_WHT,0.7f), TEAL };
        int evTypeIdx = 0;
        for (int k = 0; k < 3; k++)
            if (strcmp(event_type[k], events[ei].getEventType()) == 0) { evTypeIdx = k; break; }
        DrawRectangle(cardX + 24, cardY + 62, 150, 28, Fade(etClr[evTypeIdx], 0.18f));
        DrawRectangleLines(cardX + 24, cardY + 62, 150, 28, Fade(etClr[evTypeIdx], 0.40f));
        DrawText(events[ei].getEventType(),
            cardX + 24 + (150 - MeasureText(events[ei].getEventType(), 13)) / 2, cardY + 69, 13, etClr[evTypeIdx]);

        int pkg2 = events[ei].getPackage();
        Color pkgC2 = pkg2 == 3 ? RED_C : pkg2 == 2 ? GOLD : pkg2 == 1 ? TEAL : SOFT_WHT;
        DrawRectangle(cardX + 184, cardY + 62, 130, 28, Fade(pkgC2, 0.15f));
        DrawRectangleLines(cardX + 184, cardY + 62, 130, 28, Fade(pkgC2, 0.35f));
        DrawText(event_package[pkg2],
            cardX + 184 + (130 - MeasureText(event_package[pkg2], 13)) / 2, cardY + 69, 13, pkgC2);

        int col1 = cardX + 24, col2 = cardX + 280, col3 = cardX + 520;
        int ry2 = cardY + 104;
        auto detRow2 = [&](int x, int y2, const char* lbl, const char* val, Color valC) {
            DrawText(lbl, x, y2, 11, MUTED);
            DrawText(val, x, y2 + 18, 16, valC);
            };
        detRow2(col1, ry2, "DATE", events[ei].getDate(), SOFT_WHT);
        detRow2(col1, ry2 + 50, "TIME", events[ei].getTime(), SOFT_WHT);
        char gStr[20]; sprintf(gStr, "%d guests", events[ei].getPartySize());
        detRow2(col2, ry2, "GUESTS", gStr, SOFT_WHT);

        DrawText("ADD-ONS", col2, ry2 + 50, 11, MUTED);
        int ax2 = col2, ay2 = ry2 + 68;
        if (events[ei].getDecor()) {
            DrawRectangle(ax2, ay2, 64, 20, Fade(GOLD, 0.15f));
            DrawRectangleLines(ax2, ay2, 64, 20, Fade(GOLD, 0.35f));
            DrawText("Decor", ax2 + 6, ay2 + 4, 10, GOLD); ax2 += 70;
        }
        if (events[ei].getSpecialMenu()) {
            DrawRectangle(ax2, ay2, 60, 20, Fade(TEAL, 0.15f));
            DrawRectangleLines(ax2, ay2, 60, 20, Fade(TEAL, 0.35f));
            DrawText("Menu", ax2 + 6, ay2 + 4, 10, TEAL); ax2 += 66;
        }
        if (events[ei].getDJ()) {
            DrawRectangle(ax2, ay2, 44, 20, Fade(RED_C, 0.15f));
            DrawRectangleLines(ax2, ay2, 44, 20, Fade(RED_C, 0.35f));
            DrawText("DJ", ax2 + 8, ay2 + 4, 10, RED_C);
        }
        if (!events[ei].getDecor() && !events[ei].getSpecialMenu() && !events[ei].getDJ())
            DrawText("None", col2, ry2 + 68, 11, Fade(MUTED, 0.5f));

        DrawRectangle(col3, ry2, cardX + cardW2 - 16 - col3, 100, Fade(GOLD, 0.08f));
        DrawRectangleLines(col3, ry2, cardX + cardW2 - 16 - col3, 100, Fade(GOLD, 0.25f));
        DrawText("TOTAL COST", col3 + 10, ry2 + 10, 11, Fade(GOLD, 0.65f));
        char costStr[32]; sprintf(costStr, "Rs.%.0f", events[ei].getTotalCost());
        DrawText(costStr, col3 + 10, ry2 + 30, 28, GOLD);
        DrawText("(incl. all add-ons)", col3 + 10, ry2 + 66, 10, MUTED);

        if (isCancelled)
            DrawLine(cardX + 16, cardY + cardH2 / 2, cardX + cardW2 - 16, cardY + cardH2 / 2, Fade(RED_C, 0.18f));

        DrawLine(cardX + 16, cardY + 220, cardX + cardW2 - 16, cardY + 220, Fade(RED_C, 0.15f));
        DrawRectangle(cardX + 16, cardY + 230, cardW2 - 32, 40, Fade(BG, 0.6f));
        char sumBuf2[180];
        sprintf(sumBuf2, "Event #%d  |  %s  |  Pkg: %s  |  %s  |  %s  |  Guests: %d  |  Total: Rs.%.0f",
            events[ei].getID(), events[ei].getEventType(), event_package[pkg2],
            events[ei].getDate(), events[ei].getTime(),
            events[ei].getPartySize(), events[ei].getTotalCost());
        DrawText(sumBuf2, cardX + 24, cardY + 245, 11, MUTED);
        DrawLine(cardX + 16, cardY + 282, cardX + cardW2 - 16, cardY + 282, Fade(RED_C, 0.15f));

        if (!isCancelled) {
            if (search_cancelConfirmEvt != ei) {
                if (OutlineBtn(cardX + 24, cardY + 300, 180, 40, "Cancel This Event", RED_C, 12))
                    search_cancelConfirmEvt = ei;
                DrawText("To keep, simply do nothing.", cardX + 220, cardY + 314, 12, Fade(MUTED, 0.55f));
            }
            else {
                DrawText("Are you sure you want to cancel this event?", cardX + 24, cardY + 300, 13, RED_C);
                if (Button(cardX + 24, cardY + 322, 110, 36, "Yes, Cancel", RED_C, 12)) cancelEvent(ei);
                if (OutlineBtn(cardX + 148, cardY + 322, 90, 36, "Keep It", TEAL, 12))   search_cancelConfirmEvt = -1;
            }
        }
        else {
            DrawRectangle(cardX + 24, cardY + 300, cardW2 - 48, 40, Fade(RED_C, 0.08f));
            DrawText("This event has been cancelled. No further action available.",
                cardX + 32, cardY + 316, 12, Fade(RED_C, 0.65f));
        }
    }

    // Not found state
    else if ((search_tab == 0 && search_resIDX == -1) || (search_tab == 1 && search_evtIDX == -1)) {
        int cy = sh / 2 + 30;
        DrawCircle(sw / 2, cy - 20, 38, Fade(RED_C, 0.08f));
        DrawCircleLines(sw / 2, cy - 20, 38, Fade(RED_C, 0.30f));
        DrawText("X", sw / 2 - 8, cy - 34, 24, Fade(RED_C, 0.55f));
        const char* nf = search_tab == 0
            ? "No reservation matches that ID."
            : "No event booking matches that ID.";
        DrawText(nf, sw / 2 - MeasureText(nf, 16) / 2, cy + 26, 16, MUTED);
        DrawText("Double-check the ID from your booking confirmation.",
            sw / 2 - MeasureText("Double-check the ID from your booking confirmation.", 12) / 2,
            cy + 52, 12, Fade(MUTED, 0.55f));
    }
}
void ReservationManager::update()
{
    if (currentScreen == 5) drawModeSelect();
    else if (currentScreen == 0) drawFloorPlan();
    else if (currentScreen == 1) drawBookForm();
    else if (currentScreen == 6) drawWalkIn();
    else if (currentScreen == 2) drawEventForm();
    else if (currentScreen == 3) drawResList();
    else if (currentScreen == 4) drawEvtList();
    else if (currentScreen == 7) drawSearchByID();
}