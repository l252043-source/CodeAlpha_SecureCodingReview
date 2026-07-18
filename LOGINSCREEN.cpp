#include "LoginScreen.h"

extern Font fontRegular;
extern Font fontBold;

// ================================
// HELPERS
// ================================

static void typeIntoField(char* buf, int& len, int& cursor) {
    if (IsKeyPressed(KEY_LEFT) && cursor > 0)  cursor--;
    if (IsKeyPressed(KEY_RIGHT) && cursor < len) cursor++;
    if (IsKeyPressed(KEY_HOME))                  cursor = 0;
    if (IsKeyPressed(KEY_END))                   cursor = len;

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && len < 63) {
            for (int i = len; i > cursor; i--)
                buf[i] = buf[i - 1];
            buf[cursor] = (char)key;
            len++;
            cursor++;
            buf[len] = 0;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && cursor > 0) {
        for (int i = cursor - 1; i < len - 1; i++)
            buf[i] = buf[i + 1];
        len--;
        cursor--;
        buf[len] = 0;
    }

    if (IsKeyPressed(KEY_DELETE) && cursor < len) {
        for (int i = cursor; i < len - 1; i++)
            buf[i] = buf[i + 1];
        len--;
        buf[len] = 0;
    }
}

// returns character index closest to clicked x position in a field
static int getCursorFromClick(char* buf, int len, float clickX,
    float fieldX, Font fontR) {
    float relX = clickX - fieldX - 14;
    if (relX <= 0) return 0;
    for (int i = 1; i <= len; i++) {
        char temp[64];
        for (int j = 0; j < i; j++) temp[j] = buf[j];
        temp[i] = 0;
        float w = MeasureTextEx(fontR, temp, 16, 1).x;
        if (w >= relX) return i - 1;
    }
    return len;
}

// same but for password dots
static int getCursorFromClickDots(int len, float clickX,
    float fieldX, Font fontR) {
    float relX = clickX - fieldX - 14;
    if (relX <= 0) return 0;
    for (int i = 1; i <= len; i++) {
        char dots[64];
        for (int j = 0; j < i; j++) dots[j] = '*';
        dots[i] = 0;
        float w = MeasureTextEx(fontR, dots, 16, 1).x;
        if (w >= relX) return i - 1;
    }
    return len;
}

// ================================
// CONSTRUCTOR
// ================================

LoginScreen::LoginScreen() {
    for (int i = 0; i < 64; i++) {
        usernameBuffer[i] = 0;
        passwordBuffer[i] = 0;
    }
    usernameLen = 0;
    passwordLen = 0;
    usernameCursorPos = 0;
    passwordCursorPos = 0;
    usernameActive = false;
    passwordActive = false;
    showPassword = false;
    isStaff = false;
    isError = false;
    loginHover = false;
    signupHover = false;
    customerToggleHover = false;
    staffToggleHover = false;
    cursorTimer = 0.0f;
    cursorVisible = true;
    for (int i = 0; i < 128; i++) message[i] = 0;
}

// ================================
// UPDATE
// ================================

Screen LoginScreen::update(Customer& loggedCustomer, Staff& loggedStaff) {
    Vector2 mouse = GetMousePosition();
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float cx = W / 2.0f;
    float startY = H * 0.45f;

    customerToggle = { cx - 190, startY,        184, 44 };
    staffToggle = { cx + 6,   startY,        184, 44 };
    usernameBox = { cx - 190, startY + 70,   380, 50 };
    passwordBox = { cx - 190, startY + 148,  380, 50 };
    loginBtn = { cx - 190, startY + 230,  380, 52 };
    signupBtn = { cx - 190, startY + 294,  380, 52 };

    // cursor blink
    cursorTimer += GetFrameTime();
    if (cursorTimer >= 0.5f) {
        cursorTimer = 0.0f;
        cursorVisible = !cursorVisible;
    }

    // role toggle
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, customerToggle)) isStaff = false;
        if (CheckCollisionPointRec(mouse, staffToggle))    isStaff = true;
    }

    // field clicks — also set cursor position from click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool clickedUsername = CheckCollisionPointRec(mouse, usernameBox);
        bool clickedPassword = CheckCollisionPointRec(mouse, passwordBox);

        if (clickedUsername) {
            usernameActive = true;
            passwordActive = false;
            usernameCursorPos = getCursorFromClick(
                usernameBuffer, usernameLen,
                mouse.x, usernameBox.x, fontRegular);
        }
        else if (clickedPassword) {
            passwordActive = true;
            usernameActive = false;
            if (showPassword)
                passwordCursorPos = getCursorFromClick(
                    passwordBuffer, passwordLen,
                    mouse.x, passwordBox.x, fontRegular);
            else
                passwordCursorPos = getCursorFromClickDots(
                    passwordLen, mouse.x, passwordBox.x, fontRegular);
        }
        else {
            // clicked elsewhere — deactivate both
            if (!CheckCollisionPointRec(mouse, loginBtn) &&
                !CheckCollisionPointRec(mouse, signupBtn) &&
                !CheckCollisionPointRec(mouse, customerToggle) &&
                !CheckCollisionPointRec(mouse, staffToggle)) {
                usernameActive = false;
                passwordActive = false;
            }
        }
    }

    // typing
    if (usernameActive)
        typeIntoField(usernameBuffer, usernameLen, usernameCursorPos);

    if (passwordActive) {
        typeIntoField(passwordBuffer, passwordLen, passwordCursorPos);

        if (IsKeyPressed(KEY_ENTER)) {
            string role = isStaff ? "staff" : "customer";
            string result = auth.login(
                string(usernameBuffer),
                string(passwordBuffer),
                role, loggedCustomer, loggedStaff);
            if (result == "customer") return SCREEN_CUSTOMER_HOME;
            if (result == "staff") return SCREEN_STAFF_DASHBOARD;
            else {
                const char* err = "Invalid username or password.";
                int i = 0;
                while (err[i]) { message[i] = err[i]; i++; }
                message[i] = 0;
                isError = true;
            }
        }
    }

    // TAB switch
    if (IsKeyPressed(KEY_TAB)) {
        if (usernameActive) {
            usernameActive = false;
            passwordActive = true;
            passwordCursorPos = passwordLen;
        }
        else if (passwordActive) {
            passwordActive = false;
            usernameActive = true;
            usernameCursorPos = usernameLen;
        }
    }

    // eye icon click
    Rectangle eyeBtn = {
        passwordBox.x + passwordBox.width - 44,
        passwordBox.y + 12, 28, 26 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, eyeBtn))
        showPassword = !showPassword;

    // hover
    loginHover = CheckCollisionPointRec(mouse, loginBtn);
    signupHover = CheckCollisionPointRec(mouse, signupBtn);
    customerToggleHover = CheckCollisionPointRec(mouse, customerToggle);
    staffToggleHover = CheckCollisionPointRec(mouse, staffToggle);

    // I-beam cursor
    if (CheckCollisionPointRec(mouse, usernameBox) ||
        CheckCollisionPointRec(mouse, passwordBox))
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    else
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    // login button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && loginHover) {
        string role = isStaff ? "staff" : "customer";
        string result = auth.login(
            string(usernameBuffer),
            string(passwordBuffer),
            role, loggedCustomer, loggedStaff);
        if (result == "customer") return SCREEN_CUSTOMER_HOME;
        else if (result == "staff") return SCREEN_CUSTOMER_HOME;
        else {
            const char* err = "Invalid username or password.";
            int i = 0;
            while (err[i]) { message[i] = err[i]; i++; }
            message[i] = 0;
            isError = true;
        }
    }

    // signup button
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && signupHover)
        return SCREEN_SIGNUP;

    return SCREEN_LOGIN;
}

// ================================
// DRAW HELPER
// ================================

static void drawLoginField(
    Rectangle box, const char* label,
    char* buffer, int len, int curPos,
    bool isActive, bool isPassword, bool showPass,
    bool cursorVisible, bool hasEye, Font fontR)
{
    DrawTextEx(fontR, label,
        { box.x, box.y - 22 }, 13, 1, TEXT_MUTED);

    Color border = isActive ? INPUT_ACTIVE : INPUT_BORDER;
    DrawRectangleRec(box, INPUT_BG);
    DrawRectangleLinesEx(box, 1.5f, border);

    if (len == 0 && !isActive) {
        char ph[80] = "Enter your ";
        int pi = 11;
        const char* lbl = label;
        while (*lbl && pi < 79) { ph[pi++] = *lbl++; }
        // lowercase
        for (int i = 11; i < pi; i++)
            if (ph[i] >= 'A' && ph[i] <= 'Z') ph[i] += 32;
        ph[pi] = 0;
        DrawTextEx(fontR, ph,
            { box.x + 14, box.y + 15 }, 16, 1, TEXT_DIM);
    }
    else {
        if (isPassword && !showPass) {
            char dots[64];
            for (int i = 0; i < len; i++) dots[i] = '*';
            dots[len] = 0;
            DrawTextEx(fontR, dots,
                { box.x + 14, box.y + 15 }, 16, 1, WHITE_COLOR);
        }
        else {
            DrawTextEx(fontR, buffer,
                { box.x + 14, box.y + 15 }, 16, 1, WHITE_COLOR);
        }
    }

    // cursor at correct position
    if (isActive && cursorVisible) {
        char temp[64];
        if (isPassword && !showPass) {
            for (int i = 0; i < curPos; i++) temp[i] = '*';
        }
        else {
            for (int i = 0; i < curPos; i++) temp[i] = buffer[i];
        }
        temp[curPos] = 0;
        float tw = MeasureTextEx(fontR, temp, 16, 1).x;
        DrawRectangle(
            (int)(box.x + 14 + tw),
            (int)(box.y + 12), 2, 26, GOLD);
    }

    // eye icon
    if (hasEye) {
        int ex = (int)(box.x + box.width - 36);
        int ey = (int)(box.y + box.height / 2);
        Color eyeC = showPass ? GOLD : TEXT_DIM;
        DrawCircleLines(ex, ey, 9, eyeC);
        DrawCircle(ex, ey, 3, eyeC);
        if (!showPass)
            DrawLine(ex - 8, ey - 8, ex + 8, ey + 8, eyeC);
    }
}

// ================================
// DRAW
// ================================

void LoginScreen::draw() {
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float cx = W / 2.0f;
    float startY = H * 0.45f;

    customerToggle = { cx - 190, startY,        184, 44 };
    staffToggle = { cx + 6,   startY,        184, 44 };
    usernameBox = { cx - 190, startY + 70,   380, 50 };
    passwordBox = { cx - 190, startY + 148,  380, 50 };
    loginBtn = { cx - 190, startY + 230,  380, 52 };
    signupBtn = { cx - 190, startY + 294,  380, 52 };

    ClearBackground(BG_COLOR);
    DrawRectangle(0, 0, W, 4, GOLD);

    // logo
    int logoY = (int)(H * 0.12f);
    DrawCircleLines((int)cx, logoY, 42, GOLD);
    DrawCircle((int)cx, logoY, 38, PANEL_COLOR);
    const char* badge = "360";
    float badgeSize = 26.0f;
    Vector2 bM = MeasureTextEx(fontBold, badge, badgeSize, 1);
    Vector2 bPos = { cx - bM.x / 2 - 4, (float)logoY - bM.y / 2 };
    DrawTextEx(fontBold, badge, bPos, badgeSize, 1, GOLD);
    DrawTextEx(fontBold, "o",
        { bPos.x + bM.x + 1, bPos.y - 4 }, 14, 1, GOLD);

    // title
    const char* title = "DINE 360";
    float titleSize = 52.0f;
    Vector2 tM = MeasureTextEx(fontBold, title, titleSize, 1);
    Vector2 tPos = { cx - tM.x / 2 - 8, (float)(H * 0.21f) };
    DrawTextEx(fontBold, title, tPos, titleSize, 1, GOLD);
    DrawTextEx(fontBold, "o",
        { tPos.x + tM.x + 2, tPos.y - 6 }, 22, 1, GOLD);

    // subtitle
    const char* sub = "RESTAURANT MANAGEMENT";
    Vector2 sM = MeasureTextEx(fontRegular, sub, 16, 2);
    DrawTextEx(fontRegular, sub,
        { cx - sM.x / 2, (float)(H * 0.30f) }, 16, 2, TEXT_MUTED);

    // divider
    DrawRectangle((int)cx - 80, (int)(H * 0.36f), 160, 1, INPUT_BORDER);

    // LOGIN AS
    const char* loginAs = "LOGIN AS";
    Vector2 laM = MeasureTextEx(fontRegular, loginAs, 14, 1);
    DrawTextEx(fontRegular, loginAs,
        { cx - laM.x / 2, (float)(H * 0.40f) }, 14, 1, TEXT_MUTED);

    // customer toggle
    Color cBg = !isStaff ? GOLD : INPUT_BG;
    Color cTxt = !isStaff ? BG_COLOR : TEXT_MUTED;
    DrawRectangleRec(customerToggle, cBg);
    DrawRectangleLinesEx(customerToggle, 1.5f, !isStaff ? GOLD : INPUT_BORDER);
    Vector2 cM2 = MeasureTextEx(fontRegular, "CUSTOMER", 16, 1);
    DrawTextEx(fontRegular, "CUSTOMER", {
        customerToggle.x + customerToggle.width / 2 - cM2.x / 2,
        customerToggle.y + customerToggle.height / 2 - cM2.y / 2
        }, 16, 1, cTxt);

    // staff toggle
    Color sBg = isStaff ? GOLD : INPUT_BG;
    Color sTxtC = isStaff ? BG_COLOR : TEXT_MUTED;
    DrawRectangleRec(staffToggle, sBg);
    DrawRectangleLinesEx(staffToggle, 1.5f, isStaff ? GOLD : INPUT_BORDER);
    Vector2 sM2 = MeasureTextEx(fontRegular, "STAFF", 16, 1);
    DrawTextEx(fontRegular, "STAFF", {
        staffToggle.x + staffToggle.width / 2 - sM2.x / 2,
        staffToggle.y + staffToggle.height / 2 - sM2.y / 2
        }, 16, 1, sTxtC);

    // USERNAME field
    drawLoginField(usernameBox, "USERNAME",
        usernameBuffer, usernameLen, usernameCursorPos,
        usernameActive, false, false,
        cursorVisible, false, fontRegular);

    // PASSWORD field
    drawLoginField(passwordBox, "PASSWORD",
        passwordBuffer, passwordLen, passwordCursorPos,
        passwordActive, true, showPassword,
        cursorVisible, true, fontRegular);

    // LOGIN button
    Color lBg = loginHover ? GOLD_DIM : GOLD;
    DrawRectangleRec(loginBtn, lBg);
    Vector2 lM = MeasureTextEx(fontBold, "LOGIN", 18, 1);
    DrawTextEx(fontBold, "LOGIN", {
        loginBtn.x + loginBtn.width / 2 - lM.x / 2,
        loginBtn.y + loginBtn.height / 2 - lM.y / 2
        }, 18, 1, BG_COLOR);

    // CREATE ACCOUNT button
    Color sgB = signupHover ? GOLD : INPUT_BORDER;
    Color sgT = signupHover ? GOLD : TEXT_MUTED;
    DrawRectangleLinesEx(signupBtn, 1.5f, sgB);
    Vector2 sgM = MeasureTextEx(fontRegular, "CREATE ACCOUNT", 18, 1);
    DrawTextEx(fontRegular, "CREATE ACCOUNT", {
        signupBtn.x + signupBtn.width / 2 - sgM.x / 2,
        signupBtn.y + signupBtn.height / 2 - sgM.y / 2
        }, 18, 1, sgT);

    // message
    if (message[0] != 0) {
        Color mc = isError ? ERROR_RED : SUCCESS_GREEN;
        Vector2 mM = MeasureTextEx(fontRegular, message, 15, 1);
        DrawTextEx(fontRegular, message, {
            cx - mM.x / 2,
            signupBtn.y + signupBtn.height + 16
            }, 15, 1, mc);
    }

    // hint
    const char* hint = "TAB to switch     ENTER to login     ARROW KEYS to move cursor";
    Vector2 hM = MeasureTextEx(fontRegular, hint, 12, 1);
    DrawTextEx(fontRegular, hint,
        { cx - hM.x / 2, (float)(H - 30) }, 12, 1, TEXT_DIM);
}