#pragma once
#include <raylib.h>
#include <string>
#include <fstream>
#include <ctime>
#include "Theme.h"
#include "Cart.h"
#include "Managers.h"
#include "StockManager.h"
#include <fstream>
#include <sstream>
#include <ctime>
using namespace std;

class Screen
{
public:
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual ~Screen() {}
};

struct AppState
{
    Cart cart;
    StockManager stock;

    int currentScreen = 0;

    // ORDER 
    int orderType = 0;            // 1=Dine-In  2=Takeaway  3=Delivery
    int selectedTable = -1;

    // CUSTOMER 
    string customerName, phone, address;

    // PAYMENT
    float taxRate = 0.05f;
    float discount = 0.0f;

    int paymentMethod = 0;         // 1=Cash  2=Card
    int cashGiven = 0;
    bool paid = false;

    // UI 
    int activeField = 0;
    bool receiptGenerated = false;

    // TABLE
    static const int tableCount = 6;

    struct Table
    {
        bool occupied;
        time_t startTime;
        string type;
    };

    Table tables[tableCount];

    void initTables()
    {
        string types[tableCount] = {  "Couple","Couple","Family","Family","Friends","Friends"};

        for (int i = 0; i < tableCount; i++)
        {
            tables[i].occupied = false;
            tables[i].startTime = 0;
            tables[i].type = types[i];
        }
    }

    void saveTables()
    {
        ofstream file("tables.txt");
        if (!file) return;

        for (int i = 0; i < tableCount; i++)
        {
            file << tables[i].occupied << ",";
            file << tables[i].startTime << ",";
            file << tables[i].type << "\n";
        }
    }

    void loadTables()
    {
        ifstream file("tables.txt");

        if (!file.is_open())
        {
            initTables();
            saveTables();
            return;
        }

        for (int i = 0; i < tableCount; i++)
        {
            string line;

            if (!getline(file, line))
            {
                initTables();
                saveTables();
                return;
            }

            // ---------- MANUAL SPLIT ----------
            int pos1 = -1;
            int pos2 = -1;

            // find first comma
            for (int j = 0; j < line.length(); j++)
            {
                if (line[j] == ',')
                {
                    pos1 = j;
                    break;
                }
            }

            // find second comma
            for (int j = pos1 + 1; j < line.length(); j++)
            {
                if (line[j] == ',')
                {
                    pos2 = j;
                    break;
                }
            }

            // ---------- MANUAL SUBSTR ----------
            string occ = "";
            string time = "";
            string type = "";

            for (int j = 0; j < pos1; j++)
            {
                occ += line[j];
            }

            for (int j = pos1 + 1; j < pos2; j++)
            {
                time += line[j];
            }

            for (int j = pos2 + 1; j < line.length(); j++)
            {
                type += line[j];
            }

            // ---------- MANUAL STRING TO NUMBER ----------
            // occupied (0 or 1)
            int occupiedValue = 0;
            for (int j = 0; j < occ.length(); j++)
            {
                occupiedValue = occupiedValue * 10 + (occ[j] - '0');
            }

            // startTime (long number)
            long startTimeValue = 0;
            for (int j = 0; j < time.length(); j++)
            {
                startTimeValue = startTimeValue * 10 + (time[j] - '0');
            }

            // ---------- STORE ----------
            tables[i].occupied = occupiedValue;
            tables[i].startTime = startTimeValue;
            tables[i].type = type;
        }
    }

    void validateTableTimes()
    {
        int maxTime = 40 * 60;

        time_t now;    // time at the time of booking
        time(&now);   // current time

        for (int i = 0; i < tableCount; i++)
        {
            if (tables[i].occupied)
            {
                // time difference 
                long diff = now - tables[i].startTime;

                if (diff >= maxTime)
                {
                    tables[i].occupied = 0;
                    tables[i].startTime = 0;
                }
            }
        }

        saveTables();
    }

    void saveOrderToFile()
    {
        if (receiptGenerated) 
            return;

        ofstream file("orders.txt", ios::app);
        if (!file)
            return;

        file << "ORDER\n";

        for (int i = 0; i < cart.getCount(); i++)
        {
            file << cart.getItem(i)->getName() << ","
                << cart.getQty(i) << ","
                << cart.getItem(i)->getPrice() << "\n";
        }

        file << "TYPE:" << orderType << "\n";
        file << "STATUS:PENDING\n";
        file << "----------------------\n";

        receiptGenerated = true;  
    }

    // RESET
    void resetFlow()
    {
        orderType = 0;
        selectedTable = -1;

        customerName.clear();
        phone.clear();
        address.clear();

        paymentMethod = 0;
        cashGiven = 0;
        paid = false;

        activeField = 0;
        receiptGenerated = false;
    }

    void reset()
    {
        cart.clear();
        resetFlow();

        currentScreen = 0;
        taxRate = 0.05f;
        discount = 0.0f;

        stock.loadStock();
    }
};

// MENU 
class MenuScreen : public Screen
{
private:
    AppState& state;
    UIManager& ui;
    MenuManager& menus;

    float sidebarX = -320, targetX = -320;
    bool  sidebarOpen = false;
    int   selectedItem = -1;

    float cartScroll = 0, cartTargetScroll = 0;
    float menuScroll = 0, menuTargetScroll = 0;

    bool showEmptyWarning = false;

    static const int SW = Layout::SCREEN_W;
    static const int SH = Layout::SCREEN_H;
    static const int CX = Layout::CART_X;
    static const int CY = Layout::CART_Y;
    static const int CW = Layout::CART_W;
    static const int CH = Layout::CART_H;
    static const int CART_HEADER_H = 80;
    static const int CART_FOOTER_H = 180;

public:
    MenuScreen(AppState& s, UIManager& u, MenuManager& m)
        : state(s), ui(u), menus(m) {}

    void onEnter()
    {
        sidebarX = targetX = -320;
        sidebarOpen = false;
        selectedItem = -1;
        cartScroll = cartTargetScroll = 0;
        menuScroll = menuTargetScroll = 0;
    }

    void update() override
    {
        Vector2 mouse = GetMousePosition();
        float   wheel = GetMouseWheelMove();

        // Cart scroll
        const int cartContentY = CY + CART_HEADER_H;
        const int cartContentH = CH - CART_HEADER_H - CART_FOOTER_H;
        if (CheckCollisionPointRec(mouse, { (float)CX, (float)cartContentY,(float)CW, (float)cartContentH }))
        {
            int maxScroll = calcMaxScroll(state.cart.getCount() * 70, cartContentH);
            applyScroll(cartScroll, cartTargetScroll, wheel, maxScroll);
        }

        // Menu scroll
        if (CheckCollisionPointRec(mouse, { 350, 160, 750, 600 }))
        {
            int catCount = 0;
            if (selectedItem >= 0) 
            {
                menus.getCategory(selectedItem, catCount);
            }
            int maxScroll = calcMaxScroll(catCount * 40, 560);
            applyScroll(menuScroll, menuTargetScroll, wheel, maxScroll);
        }

        // Sidebar slide
        sidebarX += (targetX - sidebarX) * 0.12f;
    }

    void draw() override
    {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        const int cartContentY = CY + CART_HEADER_H;
        const int cartContentH = CH - CART_HEADER_H - CART_FOOTER_H;

        // Top bar
        DrawRectangle(0, 0, SW, 80, Theme::PRIMARY);
        const char* title = "DINE 360";
        Vector2 ts = MeasureTextEx(ui.font, title, 40, 2);
        DrawTextEx(ui.font, title, { (SW - ts.x) / 2.0f, 20 }, 40, 2, Theme::SECONDARY);

        // Cart panel background
        DrawRectangleRounded({ 1090, 70, 520, 840 }, 0.02f, 8, Color{ 0, 0, 0, 100 });
        DrawRectangle(CX, CY, CW, CH, Theme::DARK);
        DrawRectangleRoundedLines({ (float)CX, (float)CY, (float)CW, (float)CH }, 0.02f, 8, Theme::PRIMARY);
        Vector2 ct = MeasureTextEx(ui.font, "CART", 30, 2);
        DrawTextEx(ui.font, "CART", { CX + (CW - ct.x) / 2.0f, (float)(CY + 20) }, 30, 2, Theme::PRIMARY);

        // Sidebar
        DrawRectangle((int)sidebarX, 0, 320, SH, Theme::DARK);
        DrawTextEx(ui.font, "CATEGORIES", { sidebarX + 40, 90 }, 25, 2, Theme::SECONDARY);

        const char* cats[] = { "Starter","Karahi","BBQ","Platter","Handi","Dessert","Nashta" };
        for (int i = 0; i < 7; i++)
        {
            Rectangle r = { sidebarX + 30, 140.0f + i * 55, 260, 45 };
            if (ui.DrawFancyButton(r, cats[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                selectedItem = i;
                menuScroll = 0;
                menuTargetScroll = 0;
            }
        }

        // Menu table
        if (selectedItem >= 0)
        {
            int catCount = 0;
            MenuItem** cat = menus.getCategory(selectedItem, catCount);
            if (cat)
            {
                ui.DrawMenuTable(cat, catCount, 350, 160, state.cart, state.stock, menuScroll);
            }
        }

        // Hamburger button
        Rectangle hambBtn = { 15, 15, 50, 50 };
        bool hambHover = ui.DrawFancyButton(hambBtn, "");
        DrawTexturePro(ui.menuIcon,{ 0, 0, (float)ui.menuIcon.width, (float)ui.menuIcon.height },hambBtn, { 0, 0 }, 0, WHITE);
        if (hambHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            sidebarOpen = !sidebarOpen;
            targetX = sidebarOpen ? 0.0f : -320.0f;
        }
        if (hambHover) 
        {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        // Cart items
        const float itemX = 1150, qtyX = 1150 + 260;
        const Color textLight = { 240, 240, 240, 255 };
        int removeIndex = -1;
        Vector2 mouse = GetMousePosition();

        BeginScissorMode(CX, cartContentY, CW, cartContentH);
        for (int i = 0; i < state.cart.getCount(); i++)
        {
            int y = cartContentY + i * 60 - (int)cartScroll;
            if (y < cartContentY || y > cartContentY + cartContentH)
            {
                continue;
            }
            // Truncate long names
            string name = state.cart.getItem(i)->getName();
            string displayName = name;
            while (!displayName.empty() && MeasureTextEx(ui.font, (displayName + "...").c_str(), 20, 2).x > 200)
            {
                displayName.pop_back();
            }
            if (displayName != name) 
                displayName += "...";

            DrawRectangleRounded({ itemX - 15, (float)y - 8, 430, 45 }, 0.25f, 6,(i % 2 == 0) ? Color{ 55, 55, 55, 255 } : Color{ 40, 40, 40, 255 });

            DrawTextEx(ui.font, displayName.c_str(), { itemX, (float)y }, 20, 2, textLight);
            DrawTextEx(ui.font, TextFormat("x%d", state.cart.getQty(i)), { qtyX, (float)y }, 20, 2, textLight);

            // Remove button
            Rectangle crossBtn = { 1470, (float)y + 5, 26, 26 };
            bool crossHover = CheckCollisionPointRec(mouse, crossBtn);
            float crossScale = crossHover ? 1.15f : 1.0f;
            Rectangle scaledX = {
                crossBtn.x - (crossBtn.width * (crossScale - 1) / 2),
                crossBtn.y - (crossBtn.height * (crossScale - 1) / 2),
                crossBtn.width * crossScale,
                crossBtn.height * crossScale
            };

            if (crossHover)
            {
                DrawRectangleRounded({ scaledX.x - 2, scaledX.y - 2,scaledX.width + 4, scaledX.height + 4 }, 0.5f, 8, Color{ 255, 100, 100, 80 });
            }
            Color crossColor = crossHover ? Color{ 255, 90, 90, 255 }: Color{ 160, 50, 50, 255 };
            DrawRectangleRounded(scaledX, 0.5f, 8, crossColor);
            DrawTexturePro(ui.crossIcon,{ 0, 0, (float)ui.crossIcon.width, (float)ui.crossIcon.height },{ scaledX.x + 4, scaledX.y + 4, scaledX.width - 8, scaledX.height - 8 }, { 0, 0 }, 0, WHITE);

            if (crossHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
                removeIndex = i;
            if (crossHover)
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        EndScissorMode();

        if (removeIndex != -1)
            state.cart.remove(removeIndex);

        // Scrollbar
        ui.DrawScrollbar(1580, cartContentY, cartContentH, cartScroll, (float)(state.cart.getCount() * 60));

        if (state.cart.getCount() == 0)
            DrawTextEx(ui.font, "Cart is empty", { 1230, 220 }, 25, 2, GRAY);

        // Total + Next button
        const int totalY = CY + CH - 150;
        DrawLine(CX, totalY - 30, CX + CW, totalY - 30, Color{ 80, 80, 80, 255 });
        DrawTextEx(ui.font, "TOTAL",{ (float)(CX + 50), (float)totalY }, 25, 2, textLight);
        DrawTextEx(ui.font, TextFormat("Rs. %d", state.cart.getTotal()), { (float)(CX + 250), (float)(totalY - 5) }, 35, 2, Theme::PRIMARY);

        Rectangle nextBtn = { (float)(CX + 50), (float)(totalY + 50), 400, 55 };

        if (state.cart.getCount() == 0)
        {
            DrawRectangleRounded(nextBtn, 0.3f, 10, GRAY);
            Vector2 nt = MeasureTextEx(ui.font, "NEXT", 20, 2);
            DrawTextEx(ui.font, "NEXT",{ nextBtn.x + (nextBtn.width - nt.x) / 2,nextBtn.y + (nextBtn.height - nt.y) / 2 }, 20, 2, DARKGRAY); 
        }
        else
        {
            bool hover = ui.DrawFancyButton(nextBtn, "NEXT");
            bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
            if (hover && clicked)
            {
                showEmptyWarning = false;
                state.currentScreen = 1;
            }
        }

        if (showEmptyWarning)
            DrawTextEx(ui.font, "Cart is empty! Please add items first.",
                { 1180, 700 }, 20, 2, RED);
    }
};

// ================= SCREEN 1: ORDER SUMMARY =================
class OrderScreen : public Screen
{
private:
    AppState& state;
    UIManager& ui;

    static const int panelX = 550;
    static const int panelY = 140;
    static const int panelW = 700;
    static const int panelH = 330;
    static const int ROW_H = 38;

    float scroll = 0, targetScroll = 0;

public:
    OrderScreen(AppState& s, UIManager& u) : state(s), ui(u) {}

    void update() override
    {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, { (float)panelX, (float)panelY, (float)panelW, (float)panelH }))
        {
            int maxScroll = calcMaxScroll((state.cart.getCount() + 1) * ROW_H, panelH);
            applyScroll(scroll, targetScroll, GetMouseWheelMove(), maxScroll);
        }
    }

    void draw() override
    {
        DrawTextEx(ui.font, "ORDER SUMMARY", { 600, 70 }, 45, 2, Theme::PRIMARY);

        // Panel
        DrawRectangleRounded({ (float)panelX - 4, (float)panelY - 4, (float)panelW + 8, (float)panelH + 8 },0.03f, 8, Theme::PRIMARY);
        DrawRectangleRounded({ (float)panelX, (float)panelY,(float)panelW, (float)panelH },  0.03f, 8, Theme::CARD);

        BeginScissorMode(panelX, panelY, panelW, panelH);
        int drawY = ui.DrawCartRows(state.cart, panelX, panelW, panelY, ROW_H, panelH, (int)scroll);

        DrawLine(panelX + 10, drawY, panelX + panelW - 10, drawY,Color{ 180, 160, 80, 255 });
        drawY += 6;
        DrawTextEx(ui.font, TextFormat("TOTAL:  Rs. %d", state.cart.getTotal()),{ (float)(panelX + 15), (float)drawY }, 26, 2, Theme::PRIMARY);
        EndScissorMode();

        ui.DrawScrollbar(panelX + panelW + 6, panelY, panelH,scroll, (float)((state.cart.getCount() + 1) * ROW_H));

        // Order type selection
        const int buttonsY = panelY + panelH + 30;
        DrawTextEx(ui.font, "Select Order Type:",{ 580, (float)buttonsY }, 28, 2, Theme::TEXT);

        Rectangle dineBtn = { 580,  (float)(buttonsY + 45), 200, 55 };
        Rectangle takeBtn = { 800,  (float)(buttonsY + 45), 200, 55 };
        Rectangle delBtn = { 1020, (float)(buttonsY + 45), 200, 55 };

        bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        if (ui.DrawFancyButton(dineBtn, "Dine-In") && clicked)
            state.orderType = 1;
        if (ui.DrawFancyButton(takeBtn, "Takeaway") && clicked)
            state.orderType = 2;
        if (ui.DrawFancyButton(delBtn, "Delivery") && clicked)
            state.orderType = 3;

        if (state.orderType != 0)
        {
            const char* typeText =(state.orderType == 1) ? "Dine-In" :(state.orderType == 2) ? "Takeaway" : "Delivery";
            DrawTextEx(ui.font, TextFormat("Selected: %s", typeText),{ 580, (float)(buttonsY + 115) }, 25, 2, GREEN);
        }

        Rectangle nextBtn = { 580, (float)(buttonsY + 170), 320, 60 };
        Rectangle backBtn = { 920, (float)(buttonsY + 170), 220, 60 };

        bool nextHover = ui.DrawFancyButton(nextBtn, "PROCEED");
        bool backHover = ui.DrawFancyButton(backBtn, "BACK");

        if (backHover && clicked)
        {
            state.currentScreen = 0;
        }
        if (nextHover && clicked && state.orderType != 0)
        {
            bool allAvailable = true;

            for (int i = 0; i < state.cart.getCount(); i++)
            {
                string fullName =state.cart.getItem(i)->getName() + " (" +state.cart.getItem(i)->getSize() + ")";

                int qty = state.cart.getQty(i);

                if (state.stock.getStock(fullName) < qty)
                {
                    allAvailable = false;
                    break;
                }
            }

            if (allAvailable)
            {
                if (state.orderType == 1) 
                    state.currentScreen = 2;
                else if (state.orderType == 2)
                    state.currentScreen = 3;
                else if (state.orderType == 3)
                    state.currentScreen = 4;
            }
        }
    }
}; // ================= SCREEN 2: TABLE SELECTION =================
class TableScreen : public Screen
{
private:
    AppState& state;
    UIManager& ui;

    static const int maxSecs = 40 * 60;   // 40 minutes

    static const int gridX = 420;
    static const int gridY = 230;
    static const int spacingX = 240;
    static const int spacingY = 160;

    Rectangle tableBottons[6];
    bool popupScreen = false;

public:
    TableScreen(AppState& s, UIManager& u)
        : state(s), ui(u) {
    }

    // ================= UPDATE =================
    void update() override
    {
        time_t now = time(nullptr);

        int freeCount = 0;

        for (int i = 0; i < 6; i++)
        {
            // auto free table after time expires
            if (state.tables[i].occupied &&
                difftime(now, state.tables[i].startTime) >= maxSecs)
            {
                state.tables[i].occupied = false;
                state.tables[i].startTime = 0;
                state.saveTables();
            }

            if (!state.tables[i].occupied)
                freeCount++;
        }

        popupScreen = (freeCount == 0);
    }

    void draw() override
    {
        Vector2 mouse = GetMousePosition();

        ui.DrawHeader("SELECT TABLE", "Choose available table to continue order");

        DrawRectangleRounded( { 350, 160, 900, 520 }, 0.03f, 10, Theme::CARD );

        time_t now = time(nullptr);

        for (int i = 0; i < 6; i++)
        {
            int row = i / 3;
            int col = i % 3;

            Rectangle btn = { (float)(gridX + col * spacingX), (float)(gridY + row * spacingY), 190, 120 };

            tableBottons[i] = btn;

            bool occupied = state.tables[i].occupied;
            bool hover = CheckCollisionPointRec(mouse, btn);

            Color baseColor = occupied ? Theme::PRIMARY : Theme::HOVER;
            Color hoverColor = occupied ? Theme::PRIMARY : Theme::HOVER;

            DrawRectangleRounded(btn, 0.25f, 8, hover ? hoverColor : baseColor);

            if (hover)
            {
                DrawRectangleRoundedLines(btn, 0.25f, 8, WHITE);
            }
            if (state.selectedTable == i)
            {
                DrawRectangleRounded({ btn.x - 6, btn.y - 6, btn.width + 12, btn.height + 12 },0.3f, 8,Color{ 255, 215, 0, 140 } );
            }

            DrawTextEx(ui.font,TextFormat("TABLE %d", i + 1),{ btn.x + 15, btn.y + 15 }, 22, 2, WHITE );

            DrawTextEx(ui.font,  state.tables[i].type.c_str(), { btn.x + 15, btn.y + 45 }, 18, 2, Fade(WHITE, 0.85f) );

            if (occupied)
            {
                double remaining = maxSecs - difftime(now, state.tables[i].startTime);
                if (remaining < 0)
                    remaining = 0;

                int mins = (int)remaining / 60;
                int secs = (int)remaining % 60;

                DrawTextEx(ui.font, TextFormat("%02d:%02d LEFT", mins, secs), { btn.x + 15, btn.y + 85 }, 18, 2, BLACK );
            }
            else
            {
                DrawTextEx(ui.font, "AVAILABLE", { btn.x + 15, btn.y + 85 }, 18, 2, DARKGREEN  );
            }

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !occupied)
            {
                state.selectedTable = i;   
            }
        }

        //  CONTINUE 
        if (state.selectedTable != -1)
        {
            DrawRectangleRounded( { 500, 700, 600, 80 }, 0.2f, 8, Theme::DARK );

            DrawTextEx(ui.font, TextFormat("Selected Table: %d", state.selectedTable + 1), { 520, 725 }, 26, 2, WHITE );

            if (ui.DrawFancyButton({ 930, 715, 140, 50 }, "CONTINUE") && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                state.currentScreen = 3;
            }
        }

        // BACK 
        if (ui.DrawFancyButton({ 350, 715, 140, 50 }, "BACK") && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            state.selectedTable = -1;
            state.currentScreen = 1;
        }

        // POPUP
        if (popupScreen)
        {
            DrawRectangle(0, 0, Layout::SCREEN_W, Layout::SCREEN_H, Color{ 0, 0, 0, 180 }
            );

            DrawRectangleRounded( { 500, 300, 600, 250 }, 0.2f, 10, Theme::CARD );

            DrawTextEx(ui.font, "ALL TABLES ARE OCCUPIED", { 560, 350 },  30, 2, RED  );

            DrawTextEx(ui.font, "Please wait for a table to free.", { 540, 410 }, 20, 2, Theme::TEXT );

            if (ui.DrawFancyButton({ 700, 460, 200, 50 }, "OK") && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                popupScreen = false;
            }
        }
    }
};

// SCREEN 3: PAYMENT 
class PaymentScreen : public Screen
{
private:
    AppState& state;
    UIManager& ui;

    float scroll = 0, targetScroll = 0;

    static const int panelX = 120;
    static const int panelY = 160;
    static const int panelW = 650;
    static const int panelH = 620;

public:
    PaymentScreen(AppState& s, UIManager& u) : state(s), ui(u) {}

    void update() override
    {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, { (float)panelX, (float)panelY,  (float)panelW, (float)panelH }))
        {
            int contentH = state.cart.getCount() * 40 + 200;
            int maxScroll = calcMaxScroll(contentH, panelH);
            applyScroll(scroll, targetScroll, GetMouseWheelMove(), maxScroll);
        }
    }

    void draw() override
    {
        bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        float subtotal = (float)state.cart.getTotal();
        float tax = subtotal * state.taxRate;
        state.discount = (state.paymentMethod == 2) ? subtotal * 0.10f : 0.0f;
        float finalTotal = subtotal + tax - state.discount;

        // Header
        ui.DrawHeader("PAYMENT", TextFormat("Total Payable: Rs. %.0f", finalTotal));

        // Left panel — order details
        DrawRectangleRounded({ (float)panelX, (float)panelY,(float)panelW, (float)panelH }, 0.03f, 8, Theme::CARD);
        DrawTextEx(ui.font, "ORDER DETAILS", { 320, 180 }, 28, 2, Theme::PRIMARY);

        BeginScissorMode(panelX + 5, panelY + 5, panelW - 10, panelH - 10);

        int startY = panelY + 60;
        int y = startY - (int)scroll;

        for (int i = 0; i < state.cart.getCount(); i++)
        {
            if (y > panelY - 40 && y < panelY + panelH)
            {
                DrawTextEx(ui.font,  TextFormat("%s x%d",state.cart.getItem(i)->getName().c_str(), state.cart.getQty(i)), { 150, (float)y }, 20, 2, Theme::TEXT);

                DrawTextEx(ui.font,  TextFormat("Rs. %d",  state.cart.getItem(i)->getPrice() * state.cart.getQty(i)), { 650, (float)y }, 20, 2, Theme::PRIMARY);
            }

            y += 40;
        }

        EndScissorMode();

        // Right panel — payment method
        DrawRectangleRounded({ 830, 160, 650, 620 }, 0.03f, 8, Theme::CARD);
        DrawTextEx(ui.font, "PAYMENT METHOD", { 980, 180 }, 28, 2, Theme::PRIMARY);

        if (ui.DrawFancyButton({ 900, 260, 220, 60 }, "CASH") && clicked)
        {
            state.paymentMethod = 1; state.cashGiven = 0;
        }
        if (ui.DrawFancyButton({ 1170, 260, 220, 60 }, "CARD") && clicked)
        {
            state.paymentMethod = 2; state.cashGiven = 0;
        }

        // Cash entry
        if (state.paymentMethod == 1)
        {
            DrawTextEx(ui.font, "Enter Cash Amount:", { 900, 360 }, 22, 2, DARKGRAY);
            DrawRectangleRounded({ 900, 400, 400, 60 }, 0.2f, 8, WHITE);
            DrawTextEx(ui.font, TextFormat("%d", state.cashGiven), { 920, 415 }, 28, 2, BLACK);

            int key;
            while ((key = GetCharPressed()) > 0)
            {
                if (key >= '0' && key <= '9')
                {
                    state.cashGiven = state.cashGiven * 10 + (key - '0');
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE))
                state.cashGiven /= 10;

            int change = state.cashGiven - (int)finalTotal;
            if (change >= 0)
            {
                DrawTextEx(ui.font, TextFormat("Change: %d", change), { 900, 520 }, 26, 2, GREEN);
                state.paid = true;
            }
            else
            {
                DrawTextEx(ui.font, "Enter sufficient cash", { 900, 520 }, 20, 2, RED);
            }
        }
        // Card entry
        else if (state.paymentMethod == 2)
        {
            DrawTextEx(ui.font, "Press ENTER to confirm payment", { 900, 410 }, 20, 2, DARKGRAY);
            if (IsKeyPressed(KEY_ENTER)) state.paid = true;
        }

        // Action buttons
        const float actionY = 620;
        Rectangle backBtn = { 880, actionY, 220, 60 };
        Rectangle billBtn = { 1140, actionY, 260, 60 };

        if (ui.DrawFancyButton(backBtn, "BACK") && clicked)
        {
            int prevType = state.orderType;

            state.paid = false;
            state.paymentMethod = 0;
            state.cashGiven = 0;
            state.selectedTable = -1;
            state.receiptGenerated = false;

            if (prevType == 1) state.currentScreen = 2;
            else if (prevType == 3) state.currentScreen = 4;
            else state.currentScreen = 1;
        }

        if (state.paid)
        {
            if (ui.DrawFancyButton(billBtn, "GENERATE BILL") && clicked)
            {
                state.stock.saveStock();

                if (state.orderType == 1 && state.selectedTable != -1)
                {
                    int idx = state.selectedTable - 1;
                    state.tables[idx].occupied = true;
                    state.tables[idx].startTime = time(nullptr);
                    state.saveTables();
                }

                state.currentScreen = 5;
            }
        }
        else
        {
            DrawRectangleRounded(billBtn, 0.2f, 8, GRAY);
            DrawTextEx(ui.font, "GENERATE BILL",  { billBtn.x + 40, billBtn.y + 18 }, 20, 2, DARKGRAY);
        }
    }
};

// SCREEN 4: DELIVERY DETAILS 
class DeliveryScreen : public Screen
{
private:
    AppState& state;
    UIManager& ui;
    bool showError = false;

    bool isValidPhone(const string& p)
    {
        if (p.length() != 11)      return false;
        if (p.rfind("03", 0) != 0) return false;
        for (char c : p)
            if (!isdigit((unsigned char)c)) return false;
        return true;
    }

public:
    DeliveryScreen(AppState& s, UIManager& u) : state(s), ui(u) {}

    void update() override {}   // keyboard handled in draw() via GetCharPressed

    void draw() override
    {
        DrawTextEx(ui.font, "DELIVERY DETAILS", { 550, 100 }, 40, 2, Theme::PRIMARY);

        DrawRectangleRounded({ 450, 200, 500, 300 }, 0.05f, 10, Theme::CARD);

        Rectangle nameBox = { 500, 250, 400, 40 };
        Rectangle phoneBox = { 500, 320, 400, 40 };
        Rectangle addrBox = { 500, 390, 400, 40 };

        // Draw the three input fields using the shared helper
        ui.DrawInputField("Full Name *", state.customerName, nameBox,
            state.activeField == 0, "Enter your name...");

        ui.DrawInputField("Phone Number *", state.phone, phoneBox,
            state.activeField == 1, "03XXXXXXXXX");

        ui.DrawInputField("Delivery Address *", state.address, addrBox,
            state.activeField == 2, "Enter full address...");

        // Field focus on click
        Vector2 mouse = GetMousePosition();
        bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        if (clicked)
        {
            if (CheckCollisionPointRec(mouse, nameBox))  
                state.activeField = 0;
            if (CheckCollisionPointRec(mouse, phoneBox))
                state.activeField = 1;
            if (CheckCollisionPointRec(mouse, addrBox))  
                state.activeField = 2;
        }

        // Keyboard input
        int key;
        while ((key = GetCharPressed()) > 0)
        {
            if (state.activeField == 0)
            {
                if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') || key == ' ')
                    state.customerName += (char)key;
            }
            else if (state.activeField == 1)
            {
                if (key >= '0' && key <= '9' && state.phone.length() < 11)
                    state.phone += (char)key;
            }
            else if (state.activeField == 2)
            {
                if (key >= 32 && key <= 125)
                    state.address += (char)key;
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (state.activeField == 0 && !state.customerName.empty()) 
                state.customerName.pop_back();
            if (state.activeField == 1 && !state.phone.empty())     
                state.phone.pop_back();
            if (state.activeField == 2 && !state.address.empty())  
                state.address.pop_back();
        }

        // Next
        if (ui.DrawFancyButton({ 750, 500, 200, 60 }, "NEXT") && clicked)
        {
            if (state.customerName.empty() || state.address.empty() || !isValidPhone(state.phone))
                showError = true;
            else
            {
                showError = false;
                state.currentScreen = 3;
            }
        }

        // Back
        if (ui.DrawFancyButton({ 450, 500, 200, 60 }, "BACK") && clicked)
        {
            state.customerName.clear();
            state.phone.clear();
            state.address.clear();
            state.activeField = 0;
            state.currentScreen = 1;
        }

        if (showError)
            DrawTextEx(ui.font, "Please fill all fields correctly!", { 600, 570 }, 20, 2, RED);
    }
};

// SCREEN 5: FINAL BILL
class BillScreen : public Screen
{
private:
    AppState& state;
    UIManager& ui;

    static const int panelX = 540;
    static const int panelY = 140;
    static const int panelW = 720;
    static const int panelH = 520;
    static const int ROW_H = 38;
    static const int CALC_H = 160;

    float scroll = 0, targetScroll = 0;

    int contentH() const
    {
        return state.cart.getCount() * ROW_H + 10 + CALC_H;
    }

public:
    BillScreen(AppState& s, UIManager& u) : state(s), ui(u) {}

    void update() override
    {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, { (float)panelX, (float)panelY,  (float)panelW, (float)panelH }))
        {
            int maxScroll = calcMaxScroll(contentH(), panelH);
            applyScroll(scroll, targetScroll, GetMouseWheelMove(), maxScroll);
        }
    }

    void draw() override
    {
        if (!state.receiptGenerated)
        {
            state.cart.exportReceipt();

            for (int i = 0; i < state.cart.getCount(); i++)
            {
                string fullName = state.cart.getItem(i)->getName() + " (" +  state.cart.getItem(i)->getSize() + ")";

                state.stock.reduceStock( fullName, state.cart.getQty(i) );
            }

            state.stock.saveStock();
            state.saveOrderToFile();

            state.receiptGenerated = true;
        }

        DrawTextEx(ui.font, "FINAL BILL", { 650, 70 }, 45, 2, Theme::PRIMARY);

        float subtotal = (float)state.cart.getTotal();
        float tax = subtotal * state.taxRate;
        float finalTotal = subtotal + tax - state.discount;

        // Panel
        DrawRectangleRounded({ (float)panelX - 4, (float)panelY - 4, (float)panelW + 8, (float)panelH + 8 }, 0.03f, 8, Theme::PRIMARY);
        DrawRectangleRounded({ (float)panelX, (float)panelY, (float)panelW, (float)panelH }, 0.03f, 8, Theme::CARD);

        BeginScissorMode(panelX, panelY, panelW, panelH);

        int drawY = ui.DrawCartRows(state.cart, panelX, panelW, panelY, ROW_H, panelH, (int)scroll);
        drawY += 6;

        DrawLine(panelX + 10, drawY, panelX + panelW - 10, drawY, Color{ 180, 160, 80, 255 });
        drawY += 10;

        DrawTextEx(ui.font, TextFormat("Subtotal :  Rs. %.0f", subtotal),
            { (float)(panelX + 15), (float)drawY }, 24, 2, BLACK);
        drawY += ROW_H;

        DrawTextEx(ui.font, TextFormat("Tax (5%%) :  Rs. %.0f", tax),
            { (float)(panelX + 15), (float)drawY }, 24, 2, BLACK);
        drawY += ROW_H;

        DrawTextEx(ui.font, TextFormat("Discount :  Rs. %.0f", state.discount),
            { (float)(panelX + 15), (float)drawY }, 24, 2, BLACK);
        drawY += ROW_H + 6;

        DrawRectangle(panelX, drawY - 4, panelW, 42, Color{ 212, 175, 55, 40 });

        DrawTextEx(ui.font, TextFormat("FINAL TOTAL :  Rs. %.0f", finalTotal),
            { (float)(panelX + 15), (float)drawY }, 28, 2, Theme::PRIMARY);

        EndScissorMode();

        ui.DrawScrollbar(panelX + panelW + 6, panelY, panelH, scroll, (float)contentH());

        // Order info below panel
        int infoY = panelY + panelH + 20;

        const char* typeText = (state.orderType == 1) ? "Dine-In" : (state.orderType == 2) ? "Takeaway" : "Delivery";

        DrawTextEx(ui.font, TextFormat("Order Type: %s", typeText),
            { (float)(panelX + 15), (float)infoY }, 25, 2, DARKGREEN);
        infoY += 35;

        if (state.orderType == 1)
        {
            DrawTextEx(ui.font, TextFormat("Table No: %d", state.selectedTable),
                { (float)(panelX + 15), (float)infoY }, 25, 2, DARKGREEN);
            infoY += 35;
        }

        DrawTextEx(ui.font, "Payment Successful - Thank You!",
            { (float)(panelX + 15), (float)infoY }, 28, 2, GREEN);
        infoY += 50;

        // New order button
        if (ui.DrawFancyButton({ (float)(panelX + 15), (float)infoY, 250, 60 }, "NEW ORDER") &&  IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            state.cart.clear();
            state.reset();
        }
    }
};