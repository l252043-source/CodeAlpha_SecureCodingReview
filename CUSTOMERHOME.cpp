#include "CUSTOMERHOME.h"

extern Font fontRegular;
extern Font fontBold;

// ================================
// CONSTRUCTOR
// ================================

CustomerHome::CustomerHome() {
    customer = nullptr;
    messageTimer = 0.0f;
    messageVisible = false;
    logoutHover = false;
    placeOrderHover = false;
    reservationsHover = false;
    for (int i = 0; i < 128; i++) message[i] = 0;
    bool orderFoodRequested = false;
    bool reservationRequested = false;
    bool orderFoodHover = false;
    bool reservationHover = false;
}
bool CustomerHome::isOrderFoodRequested() {
    bool v = orderFoodRequested;
    orderFoodRequested = false;
    return v;
}
bool  CustomerHome::isReservationRequested() {
    bool v = reservationRequested;
    reservationRequested = false;
    return v;
}

void CustomerHome::setCustomer(Customer* c) {
    customer = c;
    message[0] = 0;
    messageVisible = false;
    messageTimer = 0.0f;
}

// ================================
// UPDATE
// ================================

Screen CustomerHome::update() {
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float btnY = (float)H - 76;
    orderFoodBtn = { (float)W / 2 - 210, btnY, 200, 52 };
    reservationBtn = { (float)W / 2 + 10,  btnY, 200, 52 };

    // Fix: Define 'mouse' before using it
    Vector2 mouse = GetMousePosition();
    orderFoodHover = CheckCollisionPointRec(mouse, orderFoodBtn);
    reservationHover = CheckCollisionPointRec(mouse, reservationBtn);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && orderFoodHover)
        orderFoodRequested = true;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && reservationHover)
        reservationRequested = true;
    if (!customer) return SCREEN_LOGIN;

    float cx = W / 2.0f;

    float btnW = 340.0f;
    float btnH = 58.0f;
    float btnX = cx - btnW / 2.0f;

    logoutBtn = { (float)W - 120, 14, 106, 36 };
    placeOrderBtn = { btnX, (float)H * 0.42f, btnW, btnH };
    reservationsBtn = { btnX, (float)H * 0.42f + btnH + 20, btnW, btnH };

    // message auto-clear
    if (messageVisible) {
        messageTimer += GetFrameTime();
        if (messageTimer >= 2.5f) {
            messageVisible = false;
            messageTimer = 0.0f;
        }
    }

    // hover
    logoutHover = CheckCollisionPointRec(mouse, logoutBtn);
    placeOrderHover = CheckCollisionPointRec(mouse, placeOrderBtn);
    reservationsHover = CheckCollisionPointRec(mouse, reservationsBtn);

    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (logoutHover) return SCREEN_LOGIN;

        if (placeOrderHover || reservationsHover) {
            const char* msg = "Coming Soon!";
            int i = 0;
            while (msg[i] && i < 127) { message[i] = msg[i]; i++; }
            message[i] = 0;
            messageVisible = true;
            messageTimer = 0.0f;
        }
    }

    return SCREEN_CUSTOMER_HOME;
}

// ================================
// DRAW
// ================================

void CustomerHome::draw() {
    if (!customer) return;

    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float cx = W / 2.0f;

    float btnW = 340.0f;
    float btnH = 58.0f;
    float btnX = cx - btnW / 2.0f;

    placeOrderBtn = { btnX, (float)H * 0.42f, btnW, btnH };
    reservationsBtn = { btnX, (float)H * 0.42f + btnH + 20, btnW, btnH };
    logoutBtn = { (float)W - 120, 14, 106, 36 };

    ClearBackground(BG_COLOR);
    DrawRectangle(0, 0, W, 4, GOLD);

    // ---- NAVBAR ----
    DrawRectangle(0, 0, W, 64, PANEL_COLOR);
    DrawRectangle(0, 63, W, 1, INPUT_BORDER);

    // logo
    DrawTextEx(fontBold, "DINE 360",
        { 20, 18 }, 22, 1, GOLD);
    DrawTextEx(fontBold, "o",
        { 20 + MeasureTextEx(fontBold, "DINE 360", 22, 1).x + 1, 12 },
        11, 1, GOLD);

    // welcome text centered in navbar
    char welcomeStr[128] = "Welcome, ";
    int wi = 9;
    const char* cname = customer->getName().c_str();
    while (cname[wi - 9] && wi < 126) {
        welcomeStr[wi] = cname[wi - 9]; wi++;
    }
    welcomeStr[wi] = 0;
    Vector2 wm = MeasureTextEx(fontRegular, welcomeStr, 15, 1);
    DrawTextEx(fontRegular, welcomeStr,
        { cx - wm.x / 2, 25 }, 15, 1, WHITE_COLOR);

    // logout button
    Color lBg = logoutHover ? ERROR_RED : Color{ 40, 60, 80, 255 };
    DrawRectangleRec(logoutBtn, lBg);
    DrawRectangleLinesEx(logoutBtn, 1,
        logoutHover ? ERROR_RED : INPUT_BORDER);
    Vector2 lm = MeasureTextEx(fontRegular, "LOGOUT", 13, 1);
    DrawTextEx(fontRegular, "LOGOUT", {
        logoutBtn.x + logoutBtn.width / 2 - lm.x / 2,
        logoutBtn.y + logoutBtn.height / 2 - lm.y / 2
        }, 13, 1, WHITE_COLOR);

    // ---- MAIN CONTENT ----

    // title
    const char* title = "CUSTOMER PORTAL";
    Vector2 tm = MeasureTextEx(fontBold, title, 30, 1);
    DrawTextEx(fontBold, title,
        { cx - tm.x / 2, (float)H * 0.16f }, 30, 1, GOLD);

    // tagline
    const char* tag =
        "Classic Fine Dining  |  Reserve  *  Celebrate  *  Remember";
    Vector2 tgm = MeasureTextEx(fontRegular, tag, 13, 1);
    DrawTextEx(fontRegular, tag,
        { cx - tgm.x / 2, (float)H * 0.16f + 42 }, 13, 1, TEXT_MUTED);

    // decorative divider
    float divY = (float)H * 0.16f + 68;
    DrawRectangle((int)cx - 120, (int)divY, 240, 1, INPUT_BORDER);
    DrawCircle((int)cx, (int)divY, 3, GOLD);
    DrawCircle((int)cx - 120, (int)divY, 2,
        Color{ 42, 64, 96, 255 });
    DrawCircle((int)cx + 120, (int)divY, 2,
        Color{ 42, 64, 96, 255 });

    // customer info strip
    float infoY = divY + 20;
    DrawRectangle((int)btnX, (int)infoY,
        (int)btnW, 48, PANEL_COLOR);
    DrawRectangleLinesEx(
        { btnX, infoY, btnW, 48 }, 1, INPUT_BORDER);
    DrawRectangle((int)btnX, (int)infoY, 3, 48, GOLD);

    DrawTextEx(fontRegular, "LOGGED IN AS",
        { btnX + 14, infoY + 6 }, 11, 1, TEXT_MUTED);
    DrawTextEx(fontBold, customer->getName().c_str(),
        { btnX + 14, infoY + 22 }, 15, 1, WHITE_COLOR);

    const char* roleStr = "Customer Account";
    Vector2 rlm = MeasureTextEx(fontRegular, roleStr, 12, 1);
    DrawTextEx(fontRegular, roleStr,
        { btnX + btnW - rlm.x - 14, infoY + 18 },
        12, 1, TEXT_DIM);

    // ---- PLACE ORDER BUTTON ----
    Color poBg = placeOrderHover ? GOLD_DIM : GOLD;
    DrawRectangleRec(placeOrderBtn, poBg);
    // left accent bar
    DrawRectangle(
        (int)placeOrderBtn.x,
        (int)placeOrderBtn.y,
        4, (int)placeOrderBtn.height,
        Color{ 160, 130, 60, 255 });

    Vector2 pm = MeasureTextEx(fontBold, "PLACE AN ORDER", 18, 1);
    DrawTextEx(fontBold, "PLACE AN ORDER", {
        placeOrderBtn.x + placeOrderBtn.width / 2 - pm.x / 2,
        placeOrderBtn.y + placeOrderBtn.height / 2 - pm.y / 2 - 6
        }, 18, 1, BG_COLOR);

    const char* poSub = "Browse menu and order your meal";
    Vector2 posm = MeasureTextEx(fontRegular, poSub, 11, 1);
    DrawTextEx(fontRegular, poSub, {
        placeOrderBtn.x + placeOrderBtn.width / 2 - posm.x / 2,
        placeOrderBtn.y + placeOrderBtn.height / 2 + pm.y / 2 - 2
        }, 11, 1, BG_COLOR);

    // ---- RESERVATIONS BUTTON ----
    Color rBg = reservationsHover ? INPUT_BG : PANEL_COLOR;
    Color rBorder = reservationsHover ? GOLD : INPUT_BORDER;
    Color rTxt = reservationsHover ? GOLD : TEXT_MUTED;
    DrawRectangleRec(reservationsBtn, rBg);
    DrawRectangleLinesEx(reservationsBtn, 1.5f, rBorder);
    // left accent bar
    DrawRectangle(
        (int)reservationsBtn.x,
        (int)reservationsBtn.y,
        4, (int)reservationsBtn.height,
        rBorder);

    Vector2 rm = MeasureTextEx(fontBold, "MAKE A RESERVATION", 18, 1);
    DrawTextEx(fontBold, "MAKE A RESERVATION", {
        reservationsBtn.x + reservationsBtn.width / 2 - rm.x / 2,
        reservationsBtn.y + reservationsBtn.height / 2 - rm.y / 2 - 6
        }, 18, 1, rTxt);

    const char* rSub = "Book a table or a private event";
    Vector2 rsm = MeasureTextEx(fontRegular, rSub, 11, 1);
    DrawTextEx(fontRegular, rSub, {
        reservationsBtn.x + reservationsBtn.width / 2 - rsm.x / 2,
        reservationsBtn.y + reservationsBtn.height / 2 + rm.y / 2 - 2
        }, 11, 1, rTxt);

    // ---- COMING SOON TOAST ----
    if (messageVisible) {
        float alpha = 1.0f;
        if (messageTimer > 1.8f)
            alpha = 1.0f - (messageTimer - 1.8f) / 0.7f;
        if (alpha < 0) alpha = 0;

        unsigned char a = (unsigned char)(alpha * 230);
        float nw = MeasureTextEx(fontBold, message, 16, 1).x + 48;
        float nx = cx - nw / 2;
        float ny = reservationsBtn.y + reservationsBtn.height + 28;

        DrawRectangle((int)nx, (int)ny, (int)nw, 44,
            Color{ 26, 46, 69, a });
        DrawRectangleLinesEx(
            { nx, ny, nw, 44 }, 1.5f,
            Color{ 224, 201, 127, a });

        // gold dot left
        DrawCircle((int)(nx + 20), (int)(ny + 22), 4,
            Color{ 224, 201, 127, a });

        Vector2 mm = MeasureTextEx(fontBold, message, 16, 1);
        DrawTextEx(fontBold, message,
            { cx - mm.x / 2, ny + 14 },
            16, 1, Color{ 224, 201, 127, a });
    }

    // ---- BOTTOM CREDIT ----
    const char* credit = "Team 4  |  BCY-2A  |  FAST-NUCES";
    Vector2 cm = MeasureTextEx(fontRegular, credit, 11, 1);
    DrawTextEx(fontRegular, credit,
        { cx - cm.x / 2, (float)(H - 24) },
        11, 1, TEXT_DIM);
    // ORDER FOOD button
    Color ofBg = orderFoodHover ? GOLD_DIM : GOLD;
    DrawRectangleRec(orderFoodBtn, ofBg);
    Vector2 ofM = MeasureTextEx(fontBold, "ORDER FOOD", 16, 1);
    DrawTextEx(fontBold, "ORDER FOOD", {
        orderFoodBtn.x + orderFoodBtn.width / 2 - ofM.x / 2,
        orderFoodBtn.y + orderFoodBtn.height / 2 - ofM.y / 2
        }, 16, 1, BG_COLOR);

    // RESERVE TABLE button
    Color rbBorder = reservationHover ? GOLD : INPUT_BORDER;
    Color rbTxt = reservationHover ? GOLD : TEXT_MUTED;
    DrawRectangleLinesEx(reservationBtn, 1.5f, rbBorder);
    Vector2 rbM = MeasureTextEx(fontRegular, "RESERVE TABLE", 16, 1);
    DrawTextEx(fontRegular, "RESERVE TABLE", {
        reservationBtn.x + reservationBtn.width / 2 - rbM.x / 2,
        reservationBtn.y + reservationBtn.height / 2 - rbM.y / 2
        }, 16, 1, rbTxt);
}