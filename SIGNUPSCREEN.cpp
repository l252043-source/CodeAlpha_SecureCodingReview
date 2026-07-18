#include "SignupScreen.h"

extern Font fontRegular;
extern Font fontBold;

// ================================
// INTERNAL HELPERS
// ================================

void SignupScreen::setMessage(const char* msg, bool error) {
    int i = 0;
    while (msg[i]) { message[i] = msg[i]; i++; }
    message[i] = 0;
    isError = error;
}

void SignupScreen::clearBuffer(char* buf, int& len) {
    for (int i = 0; i < 64; i++) buf[i] = 0;
    len = 0;
}

void SignupScreen::typeIntoBuffer(char* buf, int& len, int fieldIndex) {
    int& cursor = cursorPos[fieldIndex];

    // arrow key navigation
    if (IsKeyPressed(KEY_LEFT) && cursor > 0)   cursor--;
    if (IsKeyPressed(KEY_RIGHT) && cursor < len)  cursor++;
    if (IsKeyPressed(KEY_HOME))                   cursor = 0;
    if (IsKeyPressed(KEY_END))                    cursor = len;

    // character input — insert at cursor position
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

    // backspace — delete before cursor
    if (IsKeyPressed(KEY_BACKSPACE) && cursor > 0) {
        for (int i = cursor - 1; i < len - 1; i++)
            buf[i] = buf[i + 1];
        len--;
        cursor--;
        buf[len] = 0;
    }

    // delete — delete after cursor
    if (IsKeyPressed(KEY_DELETE) && cursor < len) {
        for (int i = cursor; i < len - 1; i++)
            buf[i] = buf[i + 1];
        len--;
        buf[len] = 0;
    }
}

int SignupScreen::getPasswordStrength() {
    if (passwordLen == 0) return 0;
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (int i = 0; i < passwordLen; i++) {
        char c = passwordBuffer[i];
        if (c >= 'A' && c <= 'Z') hasUpper = true;
        if (c >= 'a' && c <= 'z') hasLower = true;
        if (c >= '0' && c <= '9') hasDigit = true;
        if ((c >= '!' && c <= '/') ||
            (c >= ':' && c <= '@') ||
            (c >= '[' && c <= '`') ||
            (c >= '{' && c <= '~')) hasSpecial = true;
    }

    int score = (int)hasUpper + (int)hasLower +
        (int)hasDigit + (int)hasSpecial;

    if (passwordLen < 6) return 1;
    if (score <= 2)      return 1;
    if (score == 3)      return 2;
    return 3;
}

bool SignupScreen::validateFields() {
    if (nameLen == 0) {
        setMessage("Full name cannot be empty.", true);
        return false;
    }
    if (usernameLen == 0) {
        setMessage("Username cannot be empty.", true);
        return false;
    }
    if (getPasswordStrength() < 3) {
        setMessage("Password must be Strong to register.", true);
        return false;
    }
    // confirm match
    bool match = (passwordLen == confirmLen);
    if (match) {
        for (int i = 0; i < passwordLen; i++)
            if (passwordBuffer[i] != confirmBuffer[i]) { match = false; break; }
    }
    if (!match) {
        setMessage("Passwords do not match.", true);
        return false;
    }
    if (extraLen == 0) {
        setMessage(isStaff ? "Designation cannot be empty."
            : "Address cannot be empty.", true);
        return false;
    }
    if (isStaff) {
        const char* code = "DINE360ADMIN";
        bool codeMatch = (adminLen == 12);
        if (codeMatch) {
            for (int i = 0; i < 12; i++)
                if (adminBuffer[i] != code[i]) { codeMatch = false; break; }
        }
        if (!codeMatch) {
            setMessage("Invalid admin code.", true);
            return false;
        }
    }
    return true;
}

// ================================
// CONSTRUCTOR AND RESET
// ================================

SignupScreen::SignupScreen() {
    reset();
}

void SignupScreen::reset() {
    clearBuffer(nameBuffer, nameLen);
    clearBuffer(usernameBuffer, usernameLen);
    clearBuffer(passwordBuffer, passwordLen);
    clearBuffer(confirmBuffer, confirmLen);
    clearBuffer(extraBuffer, extraLen);
    clearBuffer(adminBuffer, adminLen);
    for (int i = 0; i < 128; i++) message[i] = 0;
    for (int i = 0; i < 7; i++) cursorPos[i] = 0;

    activeField = 0;
    isStaff = false;
    showPassword = false;
    showConfirm = false;
    signupBtnHover = false;
    backBtnHover = false;
    customerToggleHover = false;
    staffToggleHover = false;
    cursorTimer = 0.0f;
    cursorVisible = true;
    isError = false;
    successTimer = 0.0f;
    successTriggered = false;
}

// ================================
// RECTANGLE CALCULATION
// ================================

void calcRects(float cx, float startY, float fieldW, float fieldH,
    float gap, bool isStaff,
    Rectangle& customerToggle, Rectangle& staffToggle,
    Rectangle& nameBox, Rectangle& usernameBox,
    Rectangle& passwordBox, Rectangle& confirmBox,
    Rectangle& extraBox, Rectangle& adminBox,
    Rectangle& signupBtn, Rectangle& backBtn) {

    customerToggle = { cx - 190, startY,              184,     42 };
    staffToggle = { cx + 6,   startY,              184,     42 };
    nameBox = { cx - 190, startY + gap,        fieldW,  fieldH };
    usernameBox = { cx - 190, startY + gap * 2,    fieldW,  fieldH };
    passwordBox = { cx - 190, startY + gap * 3,    fieldW,  fieldH };
    confirmBox = { cx - 190, startY + gap * 4 + 18, fieldW, fieldH };
    extraBox = { cx - 190, startY + gap * 5 + 18, fieldW, fieldH };

    if (isStaff) {
        adminBox = { cx - 190, startY + gap * 6 + 18, fieldW, fieldH };
        signupBtn = { cx - 190, startY + gap * 7 + 18, fieldW, 52 };
        backBtn = { cx - 190, startY + gap * 7 + 82, fieldW, 52 };
    }
    else {
        adminBox = { 0, 0, 0, 0 };
        signupBtn = { cx - 190, startY + gap * 6 + 18, fieldW, 52 };
        backBtn = { cx - 190, startY + gap * 6 + 82, fieldW, 52 };
    }
}

// ================================
// DRAW HELPERS
// ================================

static void drawField(
    Rectangle box, const char* label,
    char* buffer, int len, int curPos,
    bool isActive, bool isPassword, bool showPass,
    bool cursorVisible, bool hasEye,
    Font fontR)
{
    // label
    DrawTextEx(fontR, label,
        { box.x, box.y - 22 }, 13, 1, TEXT_MUTED);

    // background and border
    Color border = isActive ? INPUT_ACTIVE : INPUT_BORDER;
    DrawRectangleRec(box, INPUT_BG);
    DrawRectangleLinesEx(box, 1.5f, border);

    // placeholder or content
    if (len == 0 && !isActive) {
        DrawTextEx(fontR, "...",
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
        float textW = MeasureTextEx(fontR, temp, 16, 1).x;
        DrawRectangle(
            (int)(box.x + 14 + textW),
            (int)(box.y + 12), 2, 24, GOLD);
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

static void drawStrengthBar(Rectangle passwordBox, int strength) {
    if (strength == 0) return;

    float barY = passwordBox.y + passwordBox.height + 6;
    float barW = (passwordBox.width - 8) / 3.0f;

    Color colors[] = { BLACK, STRENGTH_WEAK, STRENGTH_MED, STRENGTH_STRONG };
    const char* labels[] = { "", "WEAK", "MEDIUM", "STRONG" };

    for (int i = 1; i <= 3; i++) {
        Color c = (i <= strength) ? colors[strength] : INPUT_BORDER;
        DrawRectangle(
            (int)(passwordBox.x + (i - 1) * (barW + 4)),
            (int)barY, (int)barW, 5, c);
    }

    // label on right
    Color lc = colors[strength];
    DrawTextEx(GetFontDefault(), labels[strength],
        { passwordBox.x + passwordBox.width - 48, barY }, 12, 1, lc);
}

static void drawMatchIndicator(Rectangle confirmBox,
    char* pass, int passLen, char* conf, int confLen)
{
    if (confLen == 0) return;

    bool match = (passLen == confLen);
    if (match)
        for (int i = 0; i < passLen; i++)
            if (pass[i] != conf[i]) { match = false; break; }

    float ix = confirmBox.x + confirmBox.width + 14;
    float iy = confirmBox.y + confirmBox.height / 2.0f;

    if (match) {
        DrawCircle((int)ix, (int)iy, 11, SUCCESS_GREEN);
        DrawTextEx(GetFontDefault(), "OK",
            { ix - 8, iy - 7 }, 12, 1, BG_COLOR);
    }
    else {
        DrawCircle((int)ix, (int)iy, 11, ERROR_RED);
        DrawTextEx(GetFontDefault(), "X",
            { ix - 4, iy - 7 }, 12, 1, BG_COLOR);
    }
}

// ================================
// UPDATE
// ================================

Screen SignupScreen::update() {
    Vector2 mouse = GetMousePosition();
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float cx = W / 2.0f;
    float startY = H * 0.28f;
    float fieldW = 380.0f;
    float fieldH = 48.0f;
    float gap = 68.0f;

    calcRects(cx, startY, fieldW, fieldH, gap, isStaff,
        customerToggle, staffToggle,
        nameBox, usernameBox, passwordBox, confirmBox,
        extraBox, adminBox, signupBtn, backBtn);

    // success redirect
    if (successTriggered) {
        successTimer += GetFrameTime();
        if (successTimer >= 2.0f) return SCREEN_LOGIN;
        return SCREEN_SIGNUP;
    }

    // cursor blink
    cursorTimer += GetFrameTime();
    if (cursorTimer >= 0.5f) {
        cursorTimer = 0.0f;
        cursorVisible = !cursorVisible;
    }

    // role toggle
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, customerToggle)) {
            isStaff = false; activeField = 0;
        }
        if (CheckCollisionPointRec(mouse, staffToggle)) {
            isStaff = true; activeField = 0;
        }
    }

    // field clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeField = 0;
        if (CheckCollisionPointRec(mouse, nameBox))     activeField = 1;
        if (CheckCollisionPointRec(mouse, usernameBox)) activeField = 2;
        if (CheckCollisionPointRec(mouse, passwordBox)) activeField = 3;
        if (CheckCollisionPointRec(mouse, confirmBox))  activeField = 4;
        if (CheckCollisionPointRec(mouse, extraBox))    activeField = 5;
        if (isStaff && CheckCollisionPointRec(mouse, adminBox)) activeField = 6;
    }

    // TAB navigation
    if (IsKeyPressed(KEY_TAB)) {
        int maxField = isStaff ? 6 : 5;
        if (activeField == 0) activeField = 1;
        else activeField = (activeField % maxField) + 1;
    }

    // typing
    if (activeField == 1) typeIntoBuffer(nameBuffer, nameLen, 1);
    if (activeField == 2) typeIntoBuffer(usernameBuffer, usernameLen, 2);
    if (activeField == 3) typeIntoBuffer(passwordBuffer, passwordLen, 3);
    if (activeField == 4) typeIntoBuffer(confirmBuffer, confirmLen, 4);
    if (activeField == 5) typeIntoBuffer(extraBuffer, extraLen, 5);
    if (activeField == 6) typeIntoBuffer(adminBuffer, adminLen, 6);

    // eye icon clicks
    Rectangle eyePass = {
        passwordBox.x + passwordBox.width - 44,
        passwordBox.y + 12, 28, 26 };
    Rectangle eyeConf = {
        confirmBox.x + confirmBox.width - 44,
        confirmBox.y + 12, 28, 26 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, eyePass)) showPassword = !showPassword;
        if (CheckCollisionPointRec(mouse, eyeConf)) showConfirm = !showConfirm;
    }

    // hover
    signupBtnHover = CheckCollisionPointRec(mouse, signupBtn);
    backBtnHover = CheckCollisionPointRec(mouse, backBtn);
    customerToggleHover = CheckCollisionPointRec(mouse, customerToggle);
    staffToggleHover = CheckCollisionPointRec(mouse, staffToggle);

    // I-beam cursor
    bool overInput =
        CheckCollisionPointRec(mouse, nameBox) ||
        CheckCollisionPointRec(mouse, usernameBox) ||
        CheckCollisionPointRec(mouse, passwordBox) ||
        CheckCollisionPointRec(mouse, confirmBox) ||
        CheckCollisionPointRec(mouse, extraBox) ||
        (isStaff && CheckCollisionPointRec(mouse, adminBox));
    SetMouseCursor(overInput ? MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT);

    // signup button
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && signupBtnHover) {
        if (validateFields()) {
            bool ok = auth.signup(
                string(nameBuffer),
                string(usernameBuffer),
                string(passwordBuffer),
                isStaff ? "staff" : "customer",
                string(extraBuffer),
                isStaff ? string(adminBuffer) : ""
            );
            if (ok) {
                setMessage("Account created! Redirecting to login...", false);
                successTriggered = true;
                successTimer = 0.0f;
            }
            else {
                setMessage("Username already taken. Try another.", true);
            }
        }
    }

    // back button
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && backBtnHover)
        return SCREEN_LOGIN;

    return SCREEN_SIGNUP;
}

// ================================
// DRAW
// ================================

void SignupScreen::draw() {
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float cx = W / 2.0f;
    float startY = H * 0.28f;
    float fieldW = 380.0f;
    float fieldH = 48.0f;
    float gap = 68.0f;

    calcRects(cx, startY, fieldW, fieldH, gap, isStaff,
        customerToggle, staffToggle,
        nameBox, usernameBox, passwordBox, confirmBox,
        extraBox, adminBox, signupBtn, backBtn);

    ClearBackground(BG_COLOR);
    DrawRectangle(0, 0, W, 4, GOLD);

    // title
    int titleY = (int)(H * 0.06f);
    const char* title = "CREATE ACCOUNT";
    float titleSize = 32.0f;
    Vector2 tM = MeasureTextEx(fontBold, title, titleSize, 1);
    DrawTextEx(fontBold, title,
        { cx - tM.x / 2, (float)titleY }, titleSize, 1, GOLD);

    const char* sub = isStaff ? "STAFF REGISTRATION" : "CUSTOMER REGISTRATION";
    Vector2 sM = MeasureTextEx(fontRegular, sub, 14, 1);
    DrawTextEx(fontRegular, sub,
        { cx - sM.x / 2, (float)(titleY + 42) }, 14, 1, TEXT_MUTED);

    DrawRectangle((int)cx - 80, titleY + 66, 160, 1, INPUT_BORDER);

    // REGISTER AS label
    const char* regAs = "REGISTER AS";
    Vector2 rM = MeasureTextEx(fontRegular, regAs, 13, 1);
    DrawTextEx(fontRegular, regAs,
        { cx - rM.x / 2, startY - 26 }, 13, 1, TEXT_MUTED);

    // role toggles
    Color cBg = !isStaff ? GOLD : INPUT_BG;
    Color cTxt = !isStaff ? BG_COLOR : TEXT_MUTED;
    DrawRectangleRec(customerToggle, cBg);
    DrawRectangleLinesEx(customerToggle, 1.5f, !isStaff ? GOLD : INPUT_BORDER);
    Vector2 cM2 = MeasureTextEx(fontRegular, "CUSTOMER", 15, 1);
    DrawTextEx(fontRegular, "CUSTOMER", {
        customerToggle.x + customerToggle.width / 2 - cM2.x / 2,
        customerToggle.y + customerToggle.height / 2 - cM2.y / 2
        }, 15, 1, cTxt);

    Color sBg = isStaff ? GOLD : INPUT_BG;
    Color sTxtC = isStaff ? BG_COLOR : TEXT_MUTED;
    DrawRectangleRec(staffToggle, sBg);
    DrawRectangleLinesEx(staffToggle, 1.5f, isStaff ? GOLD : INPUT_BORDER);
    Vector2 sM2 = MeasureTextEx(fontRegular, "STAFF", 15, 1);
    DrawTextEx(fontRegular, "STAFF", {
        staffToggle.x + staffToggle.width / 2 - sM2.x / 2,
        staffToggle.y + staffToggle.height / 2 - sM2.y / 2
        }, 15, 1, sTxtC);

    // NAME
    drawField(nameBox, "FULL NAME",
        nameBuffer, nameLen, cursorPos[1],
        activeField == 1, false, false,
        cursorVisible, false, fontRegular);

    // USERNAME
    drawField(usernameBox, "USERNAME",
        usernameBuffer, usernameLen, cursorPos[2],
        activeField == 2, false, false,
        cursorVisible, false, fontRegular);

    // PASSWORD
    drawField(passwordBox, "PASSWORD",
        passwordBuffer, passwordLen, cursorPos[3],
        activeField == 3, true, showPassword,
        cursorVisible, true, fontRegular);

    // strength bar
    drawStrengthBar(passwordBox, getPasswordStrength());

    // CONFIRM PASSWORD
    drawField(confirmBox, "CONFIRM PASSWORD",
        confirmBuffer, confirmLen, cursorPos[4],
        activeField == 4, true, showConfirm,
        cursorVisible, true, fontRegular);

    // match indicator
    drawMatchIndicator(confirmBox,
        passwordBuffer, passwordLen,
        confirmBuffer, confirmLen);

    // EXTRA (address or designation)
    drawField(extraBox,
        isStaff ? "DESIGNATION" : "ADDRESS",
        extraBuffer, extraLen, cursorPos[5],
        activeField == 5, false, false,
        cursorVisible, false, fontRegular);

    // ADMIN CODE (staff only)
    if (isStaff) {
        drawField(adminBox, "ADMIN CODE",
            adminBuffer, adminLen, cursorPos[6],
            activeField == 6, true, false,
            cursorVisible, false, fontRegular);
    }

    // SIGNUP button
    Color sBtnBg = signupBtnHover ? GOLD_DIM : GOLD;
    DrawRectangleRec(signupBtn, sBtnBg);
    Vector2 sbM = MeasureTextEx(fontBold, "CREATE ACCOUNT", 17, 1);
    DrawTextEx(fontBold, "CREATE ACCOUNT", {
        signupBtn.x + signupBtn.width / 2 - sbM.x / 2,
        signupBtn.y + signupBtn.height / 2 - sbM.y / 2
        }, 17, 1, BG_COLOR);

    // BACK button
    Color bBorder = backBtnHover ? GOLD : INPUT_BORDER;
    Color bTxt = backBtnHover ? GOLD : TEXT_MUTED;
    DrawRectangleLinesEx(backBtn, 1.5f, bBorder);
    Vector2 bM2 = MeasureTextEx(fontRegular, "BACK TO LOGIN", 17, 1);
    DrawTextEx(fontRegular, "BACK TO LOGIN", {
        backBtn.x + backBtn.width / 2 - bM2.x / 2,
        backBtn.y + backBtn.height / 2 - bM2.y / 2
        }, 17, 1, bTxt);

    // message
    if (message[0] != 0) {
        Color msgColor = isError ? ERROR_RED : SUCCESS_GREEN;
        Vector2 mM2 = MeasureTextEx(fontRegular, message, 14, 1);
        DrawTextEx(fontRegular, message,
            { cx - mM2.x / 2,
              backBtn.y + backBtn.height + 16 },
            14, 1, msgColor);
    }

    // bottom hint
    const char* hint = "TAB to switch fields     ARROW KEYS to move cursor";
    Vector2 hM = MeasureTextEx(fontRegular, hint, 12, 1);
    DrawTextEx(fontRegular, hint,
        { cx - hM.x / 2, (float)(H - 30) }, 12, 1, TEXT_DIM);
}