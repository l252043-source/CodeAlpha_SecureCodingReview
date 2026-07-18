#include "STAFF_DASH.h"
extern Font fontRegular;
extern Font fontBold;
void StaffDashboard::showNotif(const char* msg, bool error) {
    int i = 0;
    while (msg[i] && i < 127) { notification[i] = msg[i]; i++; }
    notification[i] = 0;
    notifTimer = 0.0f;
    notifVisible = true;
    notifIsError = error;
}

void StaffDashboard::loadStock() {
    stockCount = 0;
    ifstream file("stock.txt");
    string line;
    while (getline(file, line) && stockCount < 100) {
        if (line.empty()) continue;
        // find last comma — qty is after it
        int pos = -1;
        for (int i = (int)line.size() - 1; i >= 0; i--) {
            if (line[i] == ',') { pos = i; break; }
        }
        if (pos == -1) continue;

        string name = line.substr(0, pos);
        string qStr = line.substr(pos + 1);

        // copy name
        int ni = 0;
        const char* ns = name.c_str();
        while (ns[ni] && ni < 127) {
            stock[stockCount].name[ni] = ns[ni]; ni++;
        }
        stock[stockCount].name[ni] = 0;

        // parse qty
        int val = 0;
        for (int i = 0; i < (int)qStr.size(); i++)
            if (qStr[i] >= '0' && qStr[i] <= '9')
                val = val * 10 + (qStr[i] - '0');
        stock[stockCount].qty = val;
        stockCount++;
    }
    file.close();
    unsavedChanges = false;
}

void StaffDashboard::saveStock() {
    ofstream file("stock.txt");
    for (int i = 0; i < stockCount; i++) {
        file << stock[i].name << ","
            << stock[i].qty << "\n";
    }
    file.close();
    unsavedChanges = false;
    showNotif("Stock saved successfully!", false);
}

// ================================
// FILE: ORDERS.TXT (Basit's format)
// ================================

void StaffDashboard::loadOrders() {
    orderCount = 0;
    ifstream file("orders.txt");
    string line;

    BasitOrder cur;
    cur.lineCount = 0;
    cur.type[0] = 0;
    cur.status[0] = 0;
    bool inOrder = false;

    while (getline(file, line)) {
        if (line == "ORDER") {
            inOrder = true;
            cur.lineCount = 0;
            cur.type[0] = 0;
            cur.status[0] = 0;
            continue;
        }
        if (!inOrder) continue;

        if (line == "----------------------") {
            if (orderCount < 100)
                orders[orderCount++] = cur;
            inOrder = false;
            cur.lineCount = 0;
            continue;
        }
        if (line.size() >= 5 &&
            line[0] == 'T' && line[1] == 'Y' &&
            line[2] == 'P' && line[3] == 'E' && line[4] == ':') {
            int i = 0;
            const char* s = line.c_str() + 5;
            while (*s && i < 15) { cur.type[i++] = *s++; }
            cur.type[i] = 0;
            continue;
        }
        if (line.size() >= 7 &&
            line[0] == 'S' && line[1] == 'T' &&
            line[2] == 'A' && line[3] == 'T' &&
            line[4] == 'U' && line[5] == 'S' && line[6] == ':') {
            int i = 0;
            const char* s = line.c_str() + 7;
            while (*s && i < 31) { cur.status[i++] = *s++; }
            cur.status[i] = 0;
            continue;
        }
        // item line
        if (cur.lineCount < 10) {
            int i = 0;
            const char* s = line.c_str();
            while (*s && i < 127) {
                cur.lines[cur.lineCount][i++] = *s++;
            }
            cur.lines[cur.lineCount][i] = 0;
            cur.lineCount++;
        }
    }
    file.close();
}

// ================================
// STOCK OPERATIONS
// ================================

void StaffDashboard::deleteStockEntry(int index) {
    for (int i = index; i < stockCount - 1; i++)
        stock[i] = stock[i + 1];
    stockCount--;
    confirmDeleteStock = -1;
    unsavedChanges = true;
    showNotif("Entry deleted. Press SAVE to apply.", true);
}

void StaffDashboard::addStockEntry() {
    if (addNameLen == 0 || addQtyLen == 0) {
        showNotif("Both fields are required.", true);
        return;
    }
    // check duplicate
    for (int i = 0; i < stockCount; i++) {
        bool match = true;
        for (int j = 0; addName[j] || stock[i].name[j]; j++) {
            if (addName[j] != stock[i].name[j]) {
                match = false; break;
            }
        }
        if (match) {
            showNotif("Item already exists.", true);
            return;
        }
    }
    // parse qty
    int val = 0;
    for (int i = 0; i < addQtyLen; i++)
        if (addQty[i] >= '0' && addQty[i] <= '9')
            val = val * 10 + (addQty[i] - '0');

    // copy name
    int ni = 0;
    while (addName[ni] && ni < 127) {
        stock[stockCount].name[ni] = addName[ni]; ni++;
    }
    stock[stockCount].name[ni] = 0;
    stock[stockCount].qty = val;
    stockCount++;

    unsavedChanges = true;
    showAddForm = false;
    clearAddForm();
    showNotif("Item added. Press SAVE to apply.", false);
}

// ================================
// FORM HELPERS
// ================================

void StaffDashboard::clearAddForm() {
    addName[0] = 0; addNameLen = 0;
    addQty[0] = 0; addQtyLen = 0;
    activeAddField = 0;
    addCursorPos[0] = 0;
    addCursorPos[1] = 0;
    addCursorPos[2] = 0;
}

void StaffDashboard::typeField(
    char* buf, int& len,
    int* cursorArr, int fieldIdx, int maxLen)
{
    int& cursor = cursorArr[fieldIdx];
    if (IsKeyPressed(KEY_LEFT) && cursor > 0)  cursor--;
    if (IsKeyPressed(KEY_RIGHT) && cursor < len) cursor++;
    if (IsKeyPressed(KEY_HOME))                  cursor = 0;
    if (IsKeyPressed(KEY_END))                   cursor = len;

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && len < maxLen - 1) {
            for (int i = len; i > cursor; i--)
                buf[i] = buf[i - 1];
            buf[cursor] = (char)key;
            len++; cursor++;
            buf[len] = 0;
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && cursor > 0) {
        for (int i = cursor - 1; i < len - 1; i++)
            buf[i] = buf[i + 1];
        len--; cursor--;
        buf[len] = 0;
    }
    if (IsKeyPressed(KEY_DELETE) && cursor < len) {
        for (int i = cursor; i < len - 1; i++)
            buf[i] = buf[i + 1];
        len--;
        buf[len] = 0;
    }
}

// ================================
// DRAW FIELD HELPER
// ================================

void StaffDashboard::drawField(
    Rectangle box, const char* label,
    char* buf, int len, int curPos,
    bool isActive, Font fontR)
{
    DrawTextEx(fontR, label,
        { box.x, box.y - 18 }, 11, 1, TEXT_MUTED);
    Color border = isActive ? INPUT_ACTIVE : INPUT_BORDER;
    DrawRectangleRec(box, INPUT_BG);
    DrawRectangleLinesEx(box, 1.5f, border);
    if (len == 0 && !isActive)
        DrawTextEx(fontR, "...",
            { box.x + 10, box.y + 12 }, 14, 1, TEXT_DIM);
    else
        DrawTextEx(fontR, buf,
            { box.x + 10, box.y + 12 }, 14, 1, WHITE_COLOR);
    if (isActive && cursorVisible) {
        char temp[128];
        for (int i = 0; i < curPos; i++) temp[i] = buf[i];
        temp[curPos] = 0;
        float tw = MeasureTextEx(fontR, temp, 14, 1).x;
        DrawRectangle(
            (int)(box.x + 10 + tw),
            (int)(box.y + 9), 2, 20, GOLD);
    }
}

// ================================
// CONSTRUCTOR
// ================================

StaffDashboard::StaffDashboard() {
    staff = nullptr;
    activePanel = SPANEL_INVENTORY;
    stockCount = 0;
    orderCount = 0;
    editingStock = -1;
    editQtyBuf[0] = 0;
    editQtyLen = 0;
    hoveredStock = -1;
    confirmDeleteStock = -1;
    confirmDelYes = false;
    confirmDelNo = false;
    showAddForm = false;
    editMenuIndex = -1;
    editMenuName[0] = 0; editMenuNameLen = 0;
    editMenuQty[0] = 0; editMenuQtyLen = 0;
    activeMenuField = 0;
    hoveredOrder = -1;
    notification[0] = 0;
    notifTimer = 0.0f;
    notifVisible = false;
    notifIsError = false;
    unsavedChanges = false;
    scrollOffset = 0;
    cursorTimer = 0.0f;
    cursorVisible = true;
    logoutHover = false;
    invNavHover = false;
    ordNavHover = false;
    mnuNavHover = false;
    saveHover = false;
    addFormHover = false;
    confirmAddHover = false;
    cancelAddHover = false;
    refreshHover = false;
    for (int i = 0; i < 3; i++) addCursorPos[i] = 0;
    for (int i = 0; i < 3; i++) menuEditCursorPos[i] = 0;
    clearAddForm();
}

void StaffDashboard::setStaff(Staff* s) {
    staff = s;
    activePanel = SPANEL_INVENTORY;
    editingStock = -1;
    confirmDeleteStock = -1;
    showAddForm = false;
    editMenuIndex = -1;
    scrollOffset = 0;
    loadStock();
    loadOrders();
}

// ================================
// UPDATE
// ================================

Screen StaffDashboard::update() {
    if (!staff) return SCREEN_LOGIN;

    Vector2 mouse = GetMousePosition();
    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float sideW = 200.0f;
    float mainX = sideW;
    float mainW = (float)(W - sideW);

    // cursor blink
    cursorTimer += GetFrameTime();
    if (cursorTimer >= 0.5f) {
        cursorTimer = 0.0f;
        cursorVisible = !cursorVisible;
    }

    // notification timer
    if (notifVisible) {
        notifTimer += GetFrameTime();
        if (notifTimer >= 2.8f) notifVisible = false;
    }

    // sidebar
    invNavBtn = { 10, 180, sideW - 20, 48 };
    ordNavBtn = { 10, 238, sideW - 20, 48 };
    mnuNavBtn = { 10, 296, sideW - 20, 48 };
    logoutBtn = { 10, (float)H - 60, sideW - 20, 44 };

    invNavHover = CheckCollisionPointRec(mouse, invNavBtn);
    ordNavHover = CheckCollisionPointRec(mouse, ordNavBtn);
    mnuNavHover = CheckCollisionPointRec(mouse, mnuNavBtn);
    logoutHover = CheckCollisionPointRec(mouse, logoutBtn);

    // scroll
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        scrollOffset -= (int)(wheel * 3);
        if (scrollOffset < 0) scrollOffset = 0;
    }

    // confirm delete intercepts everything
    if (confirmDeleteStock >= 0) {
        float dw = 360, dh = 140;
        float dx = W / 2.0f - dw / 2, dy = H / 2.0f - dh / 2;
        Rectangle yBtn = { dx + dw / 2 - 110,dy + dh - 52,100,38 };
        Rectangle nBtn = { dx + dw / 2 + 10, dy + dh - 52,100,38 };
        confirmDelYes = CheckCollisionPointRec(mouse, yBtn);
        confirmDelNo = CheckCollisionPointRec(mouse, nBtn);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (confirmDelYes) deleteStockEntry(confirmDeleteStock);
            if (confirmDelNo)  confirmDeleteStock = -1;
        }
        return SCREEN_STAFF_DASHBOARD;
    }

    // sidebar nav
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (logoutHover) return SCREEN_LOGIN;
        if (invNavHover) {
            activePanel = SPANEL_INVENTORY;
            editingStock = -1;
            showAddForm = false;
            editMenuIndex = -1;
            scrollOffset = 0;
            loadStock();
        }
        if (ordNavHover) {
            activePanel = SPANEL_ORDERS;
            scrollOffset = 0;
            loadOrders();
        }
        if (mnuNavHover) {
            activePanel = SPANEL_MENU;
            editMenuIndex = -1;
            scrollOffset = 0;
            loadStock();
        }
    }

    // top bar buttons
    saveBtn = { mainX + mainW - 150, 10, 140, 38 };
    addNewBtn = { mainX + mainW - 300, 10, 140, 38 };
    refreshBtn = { mainX + mainW - 460, 10, 150, 38 };

    saveHover = CheckCollisionPointRec(mouse, saveBtn);
    addFormHover = CheckCollisionPointRec(mouse, addNewBtn);
    refreshHover = CheckCollisionPointRec(mouse, refreshBtn);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (saveHover) saveStock();
        if (refreshHover) {
            loadStock();
            loadOrders();
            showNotif("Refreshed from file.", false);
        }
        if (addFormHover &&
            activePanel == SPANEL_INVENTORY) {
            showAddForm = !showAddForm;
            clearAddForm();
            editingStock = -1;
        }
    }

    // ---- INVENTORY PANEL ----
    if (activePanel == SPANEL_INVENTORY) {
        float rowH = 52.0f;
        float startY = showAddForm ? 230.0f : 100.0f;

        if (showAddForm) {
            float fY = 110.0f, fH = 42.0f;
            float nameW = mainW * 0.55f;
            float qtyW = mainW * 0.18f;
            float fX = mainX + 16;

            Rectangle nameField = { fX,          fY, nameW, fH };
            Rectangle qtyField = { fX + nameW + 12, fY, qtyW,  fH };
            Rectangle confBtn = { mainX + mainW - 210,
                                    fY + fH + 8, 95, 34 };
            Rectangle cancBtn = { mainX + mainW - 106,
                                    fY + fH + 8, 95, 34 };

            confirmAddHover = CheckCollisionPointRec(mouse, confBtn);
            cancelAddHover = CheckCollisionPointRec(mouse, cancBtn);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                activeAddField = 0;
                if (CheckCollisionPointRec(mouse, nameField))
                    activeAddField = 1;
                if (CheckCollisionPointRec(mouse, qtyField))
                    activeAddField = 2;
                if (confirmAddHover) addStockEntry();
                if (cancelAddHover) {
                    showAddForm = false;
                    clearAddForm();
                }
            }
            if (IsKeyPressed(KEY_TAB)) {
                if (activeAddField == 0) activeAddField = 1;
                else activeAddField = (activeAddField % 2) + 1;
            }
            if (IsKeyPressed(KEY_ENTER)) addStockEntry();

            if (activeAddField == 1)
                typeField(addName, addNameLen, addCursorPos, 1, 127);
            if (activeAddField == 2)
                typeField(addQty, addQtyLen, addCursorPos, 2, 10);

            bool over =
                CheckCollisionPointRec(mouse, nameField) ||
                CheckCollisionPointRec(mouse, qtyField);
            SetMouseCursor(over ?
                MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT);
        }
        else {
            hoveredStock = -1;
            float ry = startY;
            for (int i = scrollOffset; i < stockCount; i++) {
                Rectangle row = { mainX,ry,mainW,rowH - 2 };
                if (CheckCollisionPointRec(mouse, row)) {
                    hoveredStock = i;
                    Rectangle delBtn = {
                        mainX + mainW - 82,ry + 12,72,28 };
                    Rectangle qtyArea = {
                        mainX + mainW - 220,ry + 10,110,30 };

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (CheckCollisionPointRec(mouse, delBtn))
                            confirmDeleteStock = i;
                        else if (CheckCollisionPointRec(
                            mouse, qtyArea) &&
                            editingStock != i) {
                            editingStock = i;
                            editQtyLen = 0;
                            // load current qty into buffer
                            int sv = stock[i].qty;
                            char tmp[16]; int ti = 0;
                            if (sv == 0) { tmp[ti++] = '0'; }
                            else {
                                int p = sv; while (p > 0) {
                                    tmp[ti++] = '0' + (p % 10); p /= 10;
                                }
                            }
                            for (int j = 0; j < ti / 2; j++) {
                                char t = tmp[j];
                                tmp[j] = tmp[ti - 1 - j];
                                tmp[ti - 1 - j] = t;
                            }
                            tmp[ti] = 0;
                            for (int j = 0; tmp[j]; j++)
                                editQtyBuf[editQtyLen++] = tmp[j];
                            editQtyBuf[editQtyLen] = 0;
                        }
                    }
                }
                ry += rowH;
                if (ry > H - 20) break;
            }

            // qty inline edit
            if (editingStock >= 0) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (key >= '0' && key <= '9' && editQtyLen < 8) {
                        editQtyBuf[editQtyLen++] = (char)key;
                        editQtyBuf[editQtyLen] = 0;
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && editQtyLen > 0) {
                    editQtyLen--;
                    editQtyBuf[editQtyLen] = 0;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (editQtyLen > 0) {
                        int ns = 0;
                        for (int j = 0; j < editQtyLen; j++)
                            ns = ns * 10 + (editQtyBuf[j] - '0');
                        stock[editingStock].qty = ns;
                        unsavedChanges = true;
                        showNotif("Qty updated. Press SAVE.", false);
                    }
                    editingStock = -1;
                }
                if (IsKeyPressed(KEY_ESCAPE))
                    editingStock = -1;
            }
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }

    // ---- ORDERS PANEL ----
    else if (activePanel == SPANEL_ORDERS) {
        hoveredOrder = -1;
        float rowH = 130.0f;
        float startY = 70.0f;
        float ry = startY;
        for (int i = scrollOffset; i < orderCount; i++) {
            Rectangle row = { mainX + 10,ry,mainW - 20,rowH - 4 };
            if (CheckCollisionPointRec(mouse, row))
                hoveredOrder = i;
            ry += rowH;
            if (ry > H - 20) break;
        }
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    // ---- MANAGE MENU PANEL ----
    else if (activePanel == SPANEL_MENU) {
        float rowH = 52.0f;
        float startY = 100.0f;
        hoveredStock = -1;

        if (editMenuIndex >= 0) {
            float fY = startY +
                (editMenuIndex - scrollOffset) * rowH + 6;
            float fH = 38.0f;
            float nameW = mainW * 0.55f;
            float qtyW = mainW * 0.15f;
            float fX = mainX + 14;

            Rectangle ef1 = { fX,           fY, nameW, fH };
            Rectangle ef2 = { fX + nameW + 10,  fY, qtyW,  fH };
            Rectangle sBtn = { mainX + mainW - 215,fY,100,38 };
            Rectangle cBtn = { mainX + mainW - 106,fY,100,38 };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                activeMenuField = 0;
                if (CheckCollisionPointRec(mouse, ef1))
                    activeMenuField = 1;
                if (CheckCollisionPointRec(mouse, ef2))
                    activeMenuField = 2;
                if (CheckCollisionPointRec(mouse, sBtn)) {
                    if (editMenuNameLen > 0) {
                        int ni = 0;
                        while (editMenuName[ni] && ni < 127) {
                            stock[editMenuIndex].name[ni] =
                                editMenuName[ni]; ni++;
                        }
                        stock[editMenuIndex].name[ni] = 0;
                        int nv = 0;
                        for (int j = 0; j < editMenuQtyLen; j++)
                            if (editMenuQty[j] >= '0' &&
                                editMenuQty[j] <= '9')
                                nv = nv * 10 + (editMenuQty[j] - '0');
                        stock[editMenuIndex].qty = nv;
                        unsavedChanges = true;
                        showNotif("Updated. Press SAVE.", false);
                    }
                    editMenuIndex = -1;
                }
                if (CheckCollisionPointRec(mouse, cBtn))
                    editMenuIndex = -1;
            }
            if (IsKeyPressed(KEY_TAB)) {
                if (activeMenuField == 0) activeMenuField = 1;
                else activeMenuField = (activeMenuField % 2) + 1;
            }
            if (IsKeyPressed(KEY_ENTER) && editMenuNameLen > 0) {
                int ni = 0;
                while (editMenuName[ni] && ni < 127) {
                    stock[editMenuIndex].name[ni] =
                        editMenuName[ni]; ni++;
                }
                stock[editMenuIndex].name[ni] = 0;
                int nv = 0;
                for (int j = 0; j < editMenuQtyLen; j++)
                    if (editMenuQty[j] >= '0' && editMenuQty[j] <= '9')
                        nv = nv * 10 + (editMenuQty[j] - '0');
                stock[editMenuIndex].qty = nv;
                unsavedChanges = true;
                showNotif("Updated. Press SAVE.", false);
                editMenuIndex = -1;
            }
            if (activeMenuField == 1)
                typeField(editMenuName, editMenuNameLen,
                    menuEditCursorPos, 1, 127);
            if (activeMenuField == 2)
                typeField(editMenuQty, editMenuQtyLen,
                    menuEditCursorPos, 2, 10);

            bool over = CheckCollisionPointRec(mouse, ef1) ||
                CheckCollisionPointRec(mouse, ef2);
            SetMouseCursor(over ?
                MOUSE_CURSOR_IBEAM : MOUSE_CURSOR_DEFAULT);
        }
        else {
            float ry = startY;
            for (int i = scrollOffset; i < stockCount; i++) {
                Rectangle row = { mainX + 10,ry,mainW - 20,rowH - 4 };
                if (CheckCollisionPointRec(mouse, row)) {
                    hoveredStock = i;
                    Rectangle eBtn = { mainX + mainW - 175,ry + 12,75,28 };
                    Rectangle dBtn = { mainX + mainW - 90, ry + 12,75,28 };
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (CheckCollisionPointRec(mouse, eBtn)) {
                            editMenuIndex = i;
                            activeMenuField = 0;
                            int ni = 0;
                            while (stock[i].name[ni] && ni < 127) {
                                editMenuName[ni] =
                                    stock[i].name[ni]; ni++;
                            }
                            editMenuName[ni] = 0;
                            editMenuNameLen = ni;

                            int sv = stock[i].qty;
                            editMenuQtyLen = 0;
                            char tmp[16]; int ti = 0;
                            if (sv == 0) { tmp[ti++] = '0'; }
                            else {
                                int p = sv; while (p > 0) {
                                    tmp[ti++] = '0' + (p % 10); p /= 10;
                                }
                            }
                            for (int j = 0; j < ti / 2; j++) {
                                char t = tmp[j];
                                tmp[j] = tmp[ti - 1 - j];
                                tmp[ti - 1 - j] = t;
                            }
                            tmp[ti] = 0;
                            for (int j = 0; tmp[j]; j++)
                                editMenuQty[editMenuQtyLen++] = tmp[j];
                            editMenuQty[editMenuQtyLen] = 0;

                            for (int j = 0; j < 3; j++)
                                menuEditCursorPos[j] = 0;
                            menuEditCursorPos[1] = editMenuNameLen;
                            menuEditCursorPos[2] = editMenuQtyLen;
                        }
                        if (CheckCollisionPointRec(mouse, dBtn))
                            confirmDeleteStock = i;
                    }
                }
                ry += rowH;
                if (ry > H - 20) break;
            }
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }

    return SCREEN_STAFF_DASHBOARD;
}

// ================================
// DRAW — INVENTORY PANEL
// ================================

void StaffDashboard::drawInventoryPanel(
    float mainX, float mainW, int H)
{
    float rowH = 52.0f;
    float startY = showAddForm ? 230.0f : 100.0f;

    DrawTextEx(fontBold, "INVENTORY MANAGEMENT",
        { mainX + 16,14 }, 20, 1, GOLD);

    // SAVE
    Color svBg = saveHover ? GOLD_DIM : GOLD;
    DrawRectangleRec(saveBtn, svBg);
    Vector2 svm = MeasureTextEx(fontBold, "SAVE", 14, 1);
    DrawTextEx(fontBold, "SAVE", {
        saveBtn.x + saveBtn.width / 2 - svm.x / 2,
        saveBtn.y + saveBtn.height / 2 - svm.y / 2
        }, 14, 1, BG_COLOR);

    // unsaved dot
    if (unsavedChanges) {
        DrawCircle(
            (int)(saveBtn.x + saveBtn.width + 12),
            (int)(saveBtn.y + saveBtn.height / 2),
            5, STRENGTH_MED);
        DrawTextEx(fontRegular, "Unsaved",
            { saveBtn.x + saveBtn.width + 22,saveBtn.y + 14 },
            11, 1, STRENGTH_MED);
    }

    // REFRESH
    Color rfBg = refreshHover ? INPUT_BG : PANEL_COLOR;
    DrawRectangleRec(refreshBtn, rfBg);
    DrawRectangleLinesEx(refreshBtn, 1,
        refreshHover ? GOLD : INPUT_BORDER);
    Vector2 rfm = MeasureTextEx(fontRegular, "REFRESH", 13, 1);
    DrawTextEx(fontRegular, "REFRESH", {
        refreshBtn.x + refreshBtn.width / 2 - rfm.x / 2,
        refreshBtn.y + refreshBtn.height / 2 - rfm.y / 2
        }, 13, 1, refreshHover ? GOLD : TEXT_MUTED);

    // ADD toggle
    Color addBg = addFormHover ? INPUT_BG : PANEL_COLOR;
    Color addBrd = addFormHover ? GOLD : INPUT_BORDER;
    Color addTxt = addFormHover ? GOLD : TEXT_MUTED;
    DrawRectangleRec(addNewBtn, addBg);
    DrawRectangleLinesEx(addNewBtn, 1.5f, addBrd);
    const char* addLbl = showAddForm ? "CANCEL" : "+ ADD ITEM";
    Vector2 alm = MeasureTextEx(fontRegular, addLbl, 13, 1);
    DrawTextEx(fontRegular, addLbl, {
        addNewBtn.x + addNewBtn.width / 2 - alm.x / 2,
        addNewBtn.y + addNewBtn.height / 2 - alm.y / 2
        }, 13, 1, addTxt);

    // add form
    if (showAddForm) {
        float fY = 110.0f, fH = 42.0f;
        float nameW = mainW * 0.55f;
        float qtyW = mainW * 0.18f;
        float fX = mainX + 16;

        Rectangle nameField = { fX,         fY,nameW,fH };
        Rectangle qtyField = { fX + nameW + 12,fY,qtyW, fH };
        Rectangle confBtn = { mainX + mainW - 210,fY + fH + 8,95,34 };
        Rectangle cancBtn = { mainX + mainW - 106,fY + fH + 8,95,34 };

        drawField(nameField, "ITEM NAME (e.g. Karahi (Full))",
            addName, addNameLen, addCursorPos[1],
            activeAddField == 1, fontRegular);
        drawField(qtyField, "QUANTITY",
            addQty, addQtyLen, addCursorPos[2],
            activeAddField == 2, fontRegular);

        Color cBg = confirmAddHover ? GOLD : INPUT_BG;
        Color cTxt = confirmAddHover ? BG_COLOR : GOLD;
        DrawRectangleRec(confBtn, cBg);
        DrawRectangleLinesEx(confBtn, 1, GOLD);
        Vector2 cm = MeasureTextEx(fontBold, "ADD", 13, 1);
        DrawTextEx(fontBold, "ADD", {
            confBtn.x + confBtn.width / 2 - cm.x / 2,
            confBtn.y + confBtn.height / 2 - cm.y / 2
            }, 13, 1, cTxt);

        Color cnBrd = cancelAddHover ? ERROR_RED : INPUT_BORDER;
        DrawRectangleLinesEx(cancBtn, 1, cnBrd);
        Vector2 cnm = MeasureTextEx(fontRegular, "CANCEL", 13, 1);
        DrawTextEx(fontRegular, "CANCEL", {
            cancBtn.x + cancBtn.width / 2 - cnm.x / 2,
            cancBtn.y + cancBtn.height / 2 - cnm.y / 2
            }, 13, 1, cancelAddHover ? ERROR_RED : TEXT_MUTED);

        DrawRectangle((int)mainX, (int)(fY + fH + 52),
            (int)mainW, 1, INPUT_BORDER);
    }

    // column headers
    DrawRectangle((int)mainX, (int)(startY - 44),
        (int)mainW, 36, PANEL_COLOR);
    DrawRectangle((int)mainX, (int)(startY - 9),
        (int)mainW, 1, INPUT_BORDER);
    DrawTextEx(fontRegular, "ITEM NAME",
        { mainX + 14,(float)(startY - 34) }, 11, 1, TEXT_MUTED);
    DrawTextEx(fontRegular, "QTY",
        { mainX + mainW - 220,(float)(startY - 34) }, 11, 1, TEXT_MUTED);
    DrawTextEx(fontRegular, "STATUS",
        { mainX + mainW - 140,(float)(startY - 34) }, 11, 1, TEXT_MUTED);
    DrawTextEx(fontRegular, "ACTION",
        { mainX + mainW - 82,(float)(startY - 34) }, 11, 1, TEXT_MUTED);

    // rows
    float ry = startY;
    for (int i = scrollOffset; i < stockCount; i++) {
        bool isHov = (hoveredStock == i);
        bool isEdit = (editingStock == i);

        Color sc = stock[i].qty > 10 ? SUCCESS_GREEN :
            stock[i].qty > 3 ? STRENGTH_MED :
            stock[i].qty > 0 ? ERROR_RED :
            Color{ 50,50,50,255 };

        Color rowBg = isHov ? INPUT_BG : BG_COLOR;
        DrawRectangle((int)mainX, (int)ry,
            (int)mainW, (int)(rowH - 2), rowBg);
        DrawRectangle((int)mainX, (int)(ry + rowH - 3),
            (int)mainW, 1, INPUT_BORDER);
        DrawRectangle((int)mainX, (int)ry, 3, (int)(rowH - 2), sc);

        DrawTextEx(fontBold, stock[i].name,
            { mainX + 14,ry + 17 }, 14, 1, WHITE_COLOR);

        // qty — editable
        if (isEdit) {
            Rectangle qBox = { mainX + mainW - 220,ry + 10,90,30 };
            DrawRectangleRec(qBox, INPUT_BG);
            DrawRectangleLinesEx(qBox, 1.5f, INPUT_ACTIVE);
            DrawTextEx(fontBold, editQtyBuf,
                { qBox.x + 8,qBox.y + 8 }, 13, 1, WHITE_COLOR);
            if (cursorVisible) {
                float tw = MeasureTextEx(fontBold,
                    editQtyBuf, 13, 1).x;
                DrawRectangle(
                    (int)(qBox.x + 8 + tw), (int)(qBox.y + 6),
                    2, 18, GOLD);
            }
            DrawTextEx(fontRegular, "ENTER to confirm",
                { mainX + mainW - 220,ry + 42 }, 9, 1, TEXT_DIM);
        }
        else {
            // qty number
            char qStr[16]; int qi = 0;
            int qv = stock[i].qty;
            char qtmp[16]; int qti = 0;
            if (qv == 0) { qtmp[qti++] = '0'; }
            else {
                int p = qv; while (p > 0) {
                    qtmp[qti++] = '0' + (p % 10); p /= 10;
                }
            }
            for (int j = 0; j < qti / 2; j++) {
                char t = qtmp[j]; qtmp[j] = qtmp[qti - 1 - j];
                qtmp[qti - 1 - j] = t;
            }
            qtmp[qti] = 0;
            for (int j = 0; qtmp[j]; j++) qStr[qi++] = qtmp[j];
            qStr[qi] = 0;
            DrawTextEx(fontBold, qStr,
                { mainX + mainW - 220,ry + 17 }, 14, 1, sc);

            if (isHov)
                DrawTextEx(fontRegular, "click to edit",
                    { mainX + mainW - 220,ry + 33 }, 9, 1, TEXT_DIM);
        }

        // status label
        const char* sl =
            stock[i].qty > 10 ? "IN STOCK" :
            stock[i].qty > 3 ? "LOW" :
            stock[i].qty > 0 ? "CRITICAL" : "OUT OF STOCK";
        DrawTextEx(fontRegular, sl,
            { mainX + mainW - 140,ry + 17 }, 12, 1, sc);

        // delete — hover only
        if (isHov && !isEdit) {
            Rectangle dBtn = { mainX + mainW - 82,ry + 12,72,28 };
            DrawRectangleRec(dBtn, Color{ 60,20,20,255 });
            DrawRectangleLinesEx(dBtn, 1, ERROR_RED);
            Vector2 dm = MeasureTextEx(fontRegular, "DELETE", 12, 1);
            DrawTextEx(fontRegular, "DELETE", {
                dBtn.x + dBtn.width / 2 - dm.x / 2,
                dBtn.y + dBtn.height / 2 - dm.y / 2
                }, 12, 1, ERROR_RED);
        }

        ry += rowH;
        if (ry > H - 20) break;
    }

    if (stockCount == 0)
        DrawTextEx(fontRegular, "No stock data. Press REFRESH.",
            { mainX + mainW / 2 - 100,(float)H / 2 },
            15, 1, TEXT_MUTED);

    if (stockCount > (int)((H - startY) / rowH))
        DrawTextEx(fontRegular, "scroll to see more",
            { mainX + mainW / 2 - 50,(float)(H - 20) },
            11, 1, TEXT_DIM);
}

// ================================
// DRAW — ORDERS PANEL
// ================================

void StaffDashboard::drawOrdersPanel(
    float mainX, float mainW, int H)
{
    DrawTextEx(fontBold, "INCOMING ORDERS",
        { mainX + 16,14 }, 20, 1, GOLD);

    // order count
    char cntStr[32] = "Total: ";
    int ci = 7;
    int cv = orderCount;
    char ctmp[8]; int cti = 0;
    if (cv == 0) { ctmp[cti++] = '0'; }
    else { int p = cv; while (p > 0) { ctmp[cti++] = '0' + (p % 10); p /= 10; } }
    for (int j = 0; j < cti / 2; j++) {
        char t = ctmp[j];
        ctmp[j] = ctmp[cti - 1 - j]; ctmp[cti - 1 - j] = t;
    }
    ctmp[cti] = 0;
    for (int j = 0; ctmp[j]; j++) cntStr[ci++] = ctmp[j];
    cntStr[ci] = 0;
    DrawTextEx(fontRegular, cntStr,
        { mainX + 16,42 }, 13, 1, TEXT_MUTED);

    // REFRESH
    Color rfBg = refreshHover ? INPUT_BG : PANEL_COLOR;
    DrawRectangleRec(refreshBtn, rfBg);
    DrawRectangleLinesEx(refreshBtn, 1,
        refreshHover ? GOLD : INPUT_BORDER);
    Vector2 rfm = MeasureTextEx(fontRegular, "REFRESH", 13, 1);
    DrawTextEx(fontRegular, "REFRESH", {
        refreshBtn.x + refreshBtn.width / 2 - rfm.x / 2,
        refreshBtn.y + refreshBtn.height / 2 - rfm.y / 2
        }, 13, 1, refreshHover ? GOLD : TEXT_MUTED);

    DrawRectangle((int)mainX, 58, (int)mainW, 1, INPUT_BORDER);

    if (orderCount == 0) {
        DrawTextEx(fontRegular, "No orders yet. Press REFRESH.",
            { mainX + mainW / 2 - 100,(float)H / 2 },
            15, 1, TEXT_MUTED);
        return;
    }

    float rowH = 130.0f;
    float startY = 66.0f;
    float ry = startY;

    for (int i = scrollOffset; i < orderCount && ry < H - 20; i++) {
        BasitOrder& o = orders[i];
        bool isHov = (hoveredOrder == i);

        Color sc =
            (o.status[0] == 'P') ? STRENGTH_MED :
            (o.status[0] == 'C' && o.status[1] == 'O') ?
            SUCCESS_GREEN : TEXT_DIM;

        Color rowBg = isHov ? INPUT_BG : PANEL_COLOR;
        DrawRectangle((int)mainX, (int)ry,
            (int)mainW, (int)(rowH - 4), rowBg);
        DrawRectangleLinesEx(
            { mainX,ry,mainW,rowH - 4 }, 1, INPUT_BORDER);
        DrawRectangle((int)mainX, (int)ry, 4, (int)(rowH - 4), sc);

        // order number
        char ordNum[16] = "ORDER #";
        int oi = 7;
        int ov = i + 1;
        char otmp[8]; int oti = 0;
        if (ov == 0) { otmp[oti++] = '0'; }
        else { int p = ov; while (p > 0) { otmp[oti++] = '0' + (p % 10); p /= 10; } }
        for (int j = 0; j < oti / 2; j++) {
            char t = otmp[j];
            otmp[j] = otmp[oti - 1 - j]; otmp[oti - 1 - j] = t;
        }
        otmp[oti] = 0;
        for (int j = 0; otmp[j]; j++) ordNum[oi++] = otmp[j];
        ordNum[oi] = 0;
        DrawTextEx(fontBold, ordNum,
            { mainX + 14,ry + 8 }, 15, 1, GOLD);

        // type
        const char* typeStr =
            (o.type[0] == '1') ? "Dine In" :
            (o.type[0] == '2') ? "Takeaway" : "Delivery";
        DrawTextEx(fontRegular, typeStr,
            { mainX + 120,ry + 10 }, 13, 1, TEXT_MUTED);

        // status badge
        Color sBadgeBg =
            (o.status[0] == 'P') ?
            Color{ 60,50,10,255 } :
            Color{ 10,50,20,255 };
        float sw = MeasureTextEx(fontRegular, o.status, 12, 1).x + 16;
        DrawRectangle(
            (int)(mainX + mainW - sw - 14), (int)(ry + 8),
            (int)(sw), (int)24, sBadgeBg);
        DrawTextEx(fontRegular, o.status,
            { mainX + mainW - sw - 6,ry + 10 }, 12, 1, sc);

        // item lines
        float ily = ry + 32;
        for (int j = 0; j < o.lineCount && ily < ry + rowH - 10; j++) {
            DrawTextEx(fontRegular, o.lines[j],
                { mainX + 14,ily }, 13, 1, WHITE_COLOR);
            ily += 18;
        }

        ry += rowH;
    }
}

// ================================
// DRAW — MANAGE MENU PANEL
// ================================

void StaffDashboard::drawManageMenuPanel(
    float mainX, float mainW, int H)
{
    DrawTextEx(fontBold, "MANAGE MENU",
        { mainX + 16,14 }, 20, 1, GOLD);

    Color svBg = saveHover ? GOLD_DIM : GOLD;
    DrawRectangleRec(saveBtn, svBg);
    Vector2 svm = MeasureTextEx(fontBold, "SAVE", 14, 1);
    DrawTextEx(fontBold, "SAVE", {
        saveBtn.x + saveBtn.width / 2 - svm.x / 2,
        saveBtn.y + saveBtn.height / 2 - svm.y / 2
        }, 14, 1, BG_COLOR);

    if (unsavedChanges) {
        DrawCircle(
            (int)(saveBtn.x + saveBtn.width + 12),
            (int)(saveBtn.y + saveBtn.height / 2),
            5, STRENGTH_MED);
        DrawTextEx(fontRegular, "Unsaved",
            { saveBtn.x + saveBtn.width + 22,saveBtn.y + 14 },
            11, 1, STRENGTH_MED);
    }

    // headers
    DrawRectangle((int)mainX, 56, (int)mainW, 36, PANEL_COLOR);
    DrawRectangle((int)mainX, 91, (int)mainW, 1, INPUT_BORDER);
    DrawTextEx(fontRegular, "ITEM NAME",
        { mainX + 14,66 }, 11, 1, TEXT_MUTED);
    DrawTextEx(fontRegular, "STOCK QTY",
        { mainX + mainW - 310,66 }, 11, 1, TEXT_MUTED);
    DrawTextEx(fontRegular, "ACTIONS",
        { mainX + mainW - 180,66 }, 11, 1, TEXT_MUTED);

    float rowH = 52.0f;
    float startY = 100.0f;
    float ry = startY;

    for (int i = scrollOffset; i < stockCount; i++) {
        bool isHov = (hoveredStock == i);
        bool isEdit = (editMenuIndex == i);

        Color sc =
            stock[i].qty > 10 ? SUCCESS_GREEN :
            stock[i].qty > 3 ? STRENGTH_MED :
            stock[i].qty > 0 ? ERROR_RED :
            Color{ 50,50,50,255 };

        Color rowBg = isEdit ? Color{ 20,36,54,255 } :
            isHov ? INPUT_BG : BG_COLOR;
        DrawRectangle((int)mainX, (int)ry,
            (int)mainW, (int)(rowH - 2), rowBg);
        DrawRectangle((int)mainX, (int)(ry + rowH - 3),
            (int)mainW, 1, INPUT_BORDER);
        DrawRectangle((int)mainX, (int)ry, 3, (int)(rowH - 2), sc);

        if (isEdit) {
            float fH = 38.0f;
            float nameW = mainW * 0.52f;
            float qtyW = mainW * 0.12f;
            float fX = mainX + 14;

            Rectangle ef1 = { fX,         ry + 7,nameW,fH };
            Rectangle ef2 = { fX + nameW + 10,ry + 7,qtyW, fH };
            Rectangle sBtn = { mainX + mainW - 215,ry + 7,100,38 };
            Rectangle cBtn = { mainX + mainW - 106,ry + 7,100,38 };

            // name field
            Color b1 = activeMenuField == 1 ? INPUT_ACTIVE : INPUT_BORDER;
            DrawRectangleRec(ef1, INPUT_BG);
            DrawRectangleLinesEx(ef1, 1.5f, b1);
            DrawTextEx(fontRegular, editMenuName,
                { ef1.x + 8,ef1.y + 12 }, 14, 1, WHITE_COLOR);
            if (activeMenuField == 1 && cursorVisible) {
                char tmp[128];
                int cp = menuEditCursorPos[1];
                for (int k = 0; k < cp; k++) tmp[k] = editMenuName[k];
                tmp[cp] = 0;
                float tw = MeasureTextEx(fontRegular, tmp, 14, 1).x;
                DrawRectangle(
                    (int)(ef1.x + 8 + tw), (int)(ef1.y + 9), 2, 20, GOLD);
            }

            // qty field
            Color b2 = activeMenuField == 2 ? INPUT_ACTIVE : INPUT_BORDER;
            DrawRectangleRec(ef2, INPUT_BG);
            DrawRectangleLinesEx(ef2, 1.5f, b2);
            DrawTextEx(fontRegular, editMenuQty,
                { ef2.x + 8,ef2.y + 12 }, 14, 1, WHITE_COLOR);
            if (activeMenuField == 2 && cursorVisible) {
                char tmp[16];
                int cp = menuEditCursorPos[2];
                for (int k = 0; k < cp; k++) tmp[k] = editMenuQty[k];
                tmp[cp] = 0;
                float tw = MeasureTextEx(fontRegular, tmp, 14, 1).x;
                DrawRectangle(
                    (int)(ef2.x + 8 + tw), (int)(ef2.y + 9), 2, 20, GOLD);
            }

            DrawRectangleRec(sBtn, GOLD);
            Vector2 sbm = MeasureTextEx(fontBold, "SAVE", 13, 1);
            DrawTextEx(fontBold, "SAVE", {
                sBtn.x + sBtn.width / 2 - sbm.x / 2,
                sBtn.y + sBtn.height / 2 - sbm.y / 2
                }, 13, 1, BG_COLOR);

            DrawRectangleLinesEx(cBtn, 1, INPUT_BORDER);
            Vector2 cbm = MeasureTextEx(fontRegular, "CANCEL", 13, 1);
            DrawTextEx(fontRegular, "CANCEL", {
                cBtn.x + cBtn.width / 2 - cbm.x / 2,
                cBtn.y + cBtn.height / 2 - cbm.y / 2
                }, 13, 1, TEXT_MUTED);

        }
        else {
            DrawTextEx(fontBold, stock[i].name,
                { mainX + 14,ry + 17 }, 14, 1, WHITE_COLOR);

            // qty with color
            char qStr[16]; int qi = 0;
            int qv = stock[i].qty;
            char qtmp[16]; int qti = 0;
            if (qv == 0) { qtmp[qti++] = '0'; }
            else {
                int p = qv; while (p > 0) {
                    qtmp[qti++] = '0' + (p % 10); p /= 10;
                }
            }
            for (int j = 0; j < qti / 2; j++) {
                char t = qtmp[j]; qtmp[j] = qtmp[qti - 1 - j];
                qtmp[qti - 1 - j] = t;
            }
            qtmp[qti] = 0;
            for (int j = 0; qtmp[j]; j++) qStr[qi++] = qtmp[j];
            qStr[qi] = 0;
            DrawTextEx(fontBold, qStr,
                { mainX + mainW - 310,ry + 17 }, 14, 1, sc);

            if (isHov) {
                Rectangle eBtn = { mainX + mainW - 175,ry + 12,75,28 };
                Rectangle dBtn = { mainX + mainW - 90, ry + 12,75,28 };

                DrawRectangleRec(eBtn, INPUT_BG);
                DrawRectangleLinesEx(eBtn, 1, GOLD);
                Vector2 ebm = MeasureTextEx(fontRegular, "EDIT", 12, 1);
                DrawTextEx(fontRegular, "EDIT", {
                    eBtn.x + eBtn.width / 2 - ebm.x / 2,
                    eBtn.y + eBtn.height / 2 - ebm.y / 2
                    }, 12, 1, GOLD);

                DrawRectangleRec(dBtn, Color{ 60,20,20,255 });
                DrawRectangleLinesEx(dBtn, 1, ERROR_RED);
                Vector2 dbm = MeasureTextEx(fontRegular, "DELETE", 12, 1);
                DrawTextEx(fontRegular, "DELETE", {
                    dBtn.x + dBtn.width / 2 - dbm.x / 2,
                    dBtn.y + dBtn.height / 2 - dbm.y / 2
                    }, 12, 1, ERROR_RED);
            }
        }

        ry += rowH;
        if (ry > H - 20) break;
    }

    if (stockCount > (int)((H - startY) / rowH))
        DrawTextEx(fontRegular, "scroll to see more",
            { mainX + mainW / 2 - 50,(float)(H - 20) },
            11, 1, TEXT_DIM);
}

// ================================
// DRAW — CONFIRM DELETE DIALOG
// ================================

void StaffDashboard::drawConfirmDeleteDialog(int W, int H) {
    DrawRectangle(0, 0, W, H, Color{ 0,0,0,160 });
    float dw = 400, dh = 150;
    float dx = W / 2.0f - dw / 2, dy = H / 2.0f - dh / 2;
    DrawRectangle((int)dx, (int)dy, (int)dw, (int)dh, PANEL_COLOR);
    DrawRectangleLinesEx({ dx,dy,dw,dh }, 2, ERROR_RED);

    DrawTextEx(fontBold, "DELETE ITEM?", {
        dx + dw / 2 - MeasureTextEx(fontBold,"DELETE ITEM?",18,1).x / 2,
        dy + 14 }, 18, 1, ERROR_RED);

    if (confirmDeleteStock >= 0 && confirmDeleteStock < stockCount)
        DrawTextEx(fontRegular, stock[confirmDeleteStock].name, {
            dx + dw / 2 - MeasureTextEx(fontRegular,
                stock[confirmDeleteStock].name,13,1).x / 2,
            dy + 44 }, 13, 1, WHITE_COLOR);

    DrawTextEx(fontRegular, "This will be removed from stock.txt on Save.", {
        dx + dw / 2 - MeasureTextEx(fontRegular,
            "This will be removed from stock.txt on Save.",
            11,1).x / 2,
        dy + 66 }, 11, 1, TEXT_MUTED);

    Rectangle yBtn = { dx + dw / 2 - 110,dy + dh - 52,100,38 };
    Rectangle nBtn = { dx + dw / 2 + 10, dy + dh - 52,100,38 };

    DrawRectangleRec(yBtn,
        confirmDelYes ? ERROR_RED : Color{ 60,20,20,255 });
    DrawRectangleLinesEx(yBtn, 1, ERROR_RED);
    Vector2 ym = MeasureTextEx(fontBold, "DELETE", 13, 1);
    DrawTextEx(fontBold, "DELETE", {
        yBtn.x + yBtn.width / 2 - ym.x / 2,
        yBtn.y + yBtn.height / 2 - ym.y / 2
        }, 13, 1, WHITE_COLOR);

    DrawRectangleLinesEx(nBtn, 1,
        confirmDelNo ? GOLD : INPUT_BORDER);
    Vector2 nm = MeasureTextEx(fontRegular, "CANCEL", 13, 1);
    DrawTextEx(fontRegular, "CANCEL", {
        nBtn.x + nBtn.width / 2 - nm.x / 2,
        nBtn.y + nBtn.height / 2 - nm.y / 2
        }, 13, 1, confirmDelNo ? GOLD : TEXT_MUTED);
}

// ================================
// DRAW — NOTIFICATION TOAST
// ================================

void StaffDashboard::drawNotification(int W, int H) {
    if (!notifVisible) return;
    float alpha = 1.0f;
    if (notifTimer > 2.0f)
        alpha = 1.0f - (notifTimer - 2.0f) / 0.8f;
    if (alpha < 0) alpha = 0;
    unsigned char a = (unsigned char)(alpha * 220);

    float nw = MeasureTextEx(fontRegular,
        notification, 14, 1).x + 40;
    float nx = W / 2.0f - nw / 2;
    float ny = (float)(H - 56);

    Color bgCol = notifIsError ?
        Color{ 60,20,20,a } : Color{ 26,46,69,a };
    Color brdCol = notifIsError ?
        Color{ 220,80,80,a } : Color{ 224,201,127,a };

    DrawRectangle((int)nx, (int)ny, (int)nw, 40, bgCol);
    DrawRectangleLinesEx({ nx,ny,nw,40 }, 1.5f, brdCol);
    DrawTextEx(fontRegular, notification,
        { nx + 20,ny + 13 }, 14, 1, Color{ 255,255,255,a });
}

// ================================
// DRAW — SIDEBAR
// ================================

void StaffDashboard::drawSidebar(float sideW, int H) {
    DrawRectangle(0, 0, (int)sideW, H, PANEL_COLOR);
    DrawRectangle((int)sideW - 1, 0, 1, H, INPUT_BORDER);

    DrawTextEx(fontBold, "DINE 360", { 12,16 }, 18, 1, GOLD);
    DrawTextEx(fontBold, "o", {
        12 + MeasureTextEx(fontBold,"DINE 360",18,1).x + 1,10 },
        10, 1, GOLD);

    DrawRectangle(0, 50, (int)sideW, 1, INPUT_BORDER);
    DrawTextEx(fontRegular, "STAFF", { 12,62 }, 11, 1, TEXT_MUTED);
    DrawTextEx(fontBold, staff->getName().c_str(),
        { 12,78 }, 15, 1, WHITE_COLOR);
    DrawTextEx(fontRegular, staff->getDesignation().c_str(),
        { 12,98 }, 12, 1, GOLD);
    DrawRectangle(0, 118, (int)sideW, 1, INPUT_BORDER);
    DrawTextEx(fontRegular, "NAVIGATION",
        { 12,132 }, 11, 1, TEXT_MUTED);

    struct NavE {
        const char* label;
        Rectangle   rect;
        StaffPanel  panel;
        bool        hover;
    };
    NavE navs[] = {
        {"INVENTORY", invNavBtn,SPANEL_INVENTORY,invNavHover},
        {"ORDERS",    ordNavBtn,SPANEL_ORDERS,   ordNavHover},
        {"MANAGE MENU",mnuNavBtn,SPANEL_MENU,    mnuNavHover}
    };
    for (int i = 0; i < 3; i++) {
        bool isAct = (activePanel == navs[i].panel);
        Color bg = isAct ? GOLD : navs[i].hover ? INPUT_BG : PANEL_COLOR;
        Color txt = isAct ? BG_COLOR :
            navs[i].hover ? WHITE_COLOR : TEXT_MUTED;
        Color brd = isAct ? GOLD : INPUT_BORDER;
        DrawRectangleRec(navs[i].rect, bg);
        DrawRectangleLinesEx(navs[i].rect, 1, brd);
        Vector2 nm = MeasureTextEx(fontRegular, navs[i].label, 14, 1);
        DrawTextEx(fontRegular, navs[i].label, {
            navs[i].rect.x + navs[i].rect.width / 2 - nm.x / 2,
            navs[i].rect.y + navs[i].rect.height / 2 - nm.y / 2
            }, 14, 1, txt);
    }

    Color lBg = logoutHover ? ERROR_RED : PANEL_COLOR;
    DrawRectangleRec(logoutBtn, lBg);
    DrawRectangleLinesEx(logoutBtn, 1,
        logoutHover ? ERROR_RED : INPUT_BORDER);
    Vector2 lm = MeasureTextEx(fontRegular, "LOGOUT", 14, 1);
    DrawTextEx(fontRegular, "LOGOUT", {
        logoutBtn.x + logoutBtn.width / 2 - lm.x / 2,
        logoutBtn.y + logoutBtn.height / 2 - lm.y / 2
        }, 14, 1, logoutHover ? WHITE_COLOR : TEXT_MUTED);
}

// ================================
// DRAW
// ================================

void StaffDashboard::draw() {
    if (!staff) return;

    int W = GetScreenWidth();
    int H = GetScreenHeight();
    float sideW = 200.0f;
    float mainX = sideW;
    float mainW = (float)(W - sideW);

    ClearBackground(BG_COLOR);
    DrawRectangle(0, 0, W, 4, GOLD);

    drawSidebar(sideW, H);

    // main top bar
    DrawRectangle((int)mainX, 0, (int)mainW, 54, PANEL_COLOR);
    DrawRectangle((int)mainX, 53, (int)mainW, 1, INPUT_BORDER);

    if (activePanel == SPANEL_INVENTORY)
        drawInventoryPanel(mainX, mainW, H);
    else if (activePanel == SPANEL_ORDERS)
        drawOrdersPanel(mainX, mainW, H);
    else if (activePanel == SPANEL_MENU)
        drawManageMenuPanel(mainX, mainW, H);

    if (confirmDeleteStock >= 0)
        drawConfirmDeleteDialog(W, H);
    drawNotification(W, H);
}