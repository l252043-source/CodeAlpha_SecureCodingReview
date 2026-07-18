#pragma once
#include <raylib.h>
#include "MenuItem.h"
#include "Cart.h"
#include "StockManager.h"
#include "Theme.h"
#include <sstream>

// ================= MENU DATA MANAGER =================
class MenuManager
{
public:
    static const int maxMenuItem = 20;

    MenuItem* starters[maxMenuItem]; 
    MenuItem* karahi[maxMenuItem];  
    MenuItem* bbq[maxMenuItem];      
    MenuItem* platter[maxMenuItem];  
    MenuItem* handi[maxMenuItem];    
    MenuItem* dessert[maxMenuItem];  
    MenuItem* nashta[maxMenuItem];  

    int starterCount;
    int karahiCount;
    int bbqCount;
    int platterCount;
    int handiCount;
    int dessertCount;
    int nashtaCount;

    MenuManager()  : starterCount(0), karahiCount(0), bbqCount(0),
        platterCount(0), handiCount(0), dessertCount(0), nashtaCount(0)
    {
        // Starters 
        starters[starterCount++] = new MenuItem("Thai Consome", "Single", 395);
        starters[starterCount++] = new MenuItem("Vegetable Soup", "Single", 395);
        starters[starterCount++] = new MenuItem("Hot & Sour Soup", "Single", 445);
        starters[starterCount++] = new MenuItem("Jumbo Drum Stick (4 pcs)", "Regular", 1095);
        starters[starterCount++] = new MenuItem("Zinger Drum Stick (4 pcs)", "Regular", 1095);
        starters[starterCount++] = new MenuItem("Finger Fish (8 pcs)", "Regular", 2195);

        // Karahi
        karahi[karahiCount++] = new MenuItem("Lahori Chicken Karahi", "Half", 1145);
        karahi[karahiCount++] = new MenuItem("Lahori Chicken Karahi", "Full", 1995);
        karahi[karahiCount++] = new MenuItem("Chicken Gravy Karahi", "Half", 1195);
        karahi[karahiCount++] = new MenuItem("Chicken Gravy Karahi", "Full", 2095);
        karahi[karahiCount++] = new MenuItem("Chicken Sulemani Karahi", "Full", 2195);
        karahi[karahiCount++] = new MenuItem("Chicken Makhni Karahi", "Full", 2195);
        karahi[karahiCount++] = new MenuItem("Chicken Charsi Karahi", "Full", 2195);
        karahi[karahiCount++] = new MenuItem("Lahori Mutton Karahi", "Half", 2595);
        karahi[karahiCount++] = new MenuItem("Lahori Mutton Karahi", "Full", 4995);
        karahi[karahiCount++] = new MenuItem("Mutton Gravy Karahi", "Half", 2645);
        karahi[karahiCount++] = new MenuItem("Mutton Gravy Karahi", "Full", 4995);
        karahi[karahiCount++] = new MenuItem("Mutton Sulemani Karahi", "Full", 5195);
        karahi[karahiCount++] = new MenuItem("Special Mutton Karahi", "Full", 4995);

        // BBQ
        bbq[bbqCount++] = new BBQItem("Chicken Piece (Leg/Breast)", "Regular", 795);
        bbq[bbqCount++] = new BBQItem("Fish Tikka (Sole - 8 pcs)", "Regular", 2995);
        bbq[bbqCount++] = new BBQItem("Lahori Fish Seekh Kabab", "Regular", 2995);
        bbq[bbqCount++] = new BBQItem("Chicken Gaz Kabab", "Regular", 1795);
        bbq[bbqCount++] = new BBQItem("Beef Gaz Kabab", "Regular", 1895);
        bbq[bbqCount++] = new BBQItem("Mutton Gaz Kabab", "Regular", 2595);
        bbq[bbqCount++] = new BBQItem("Veal Kabab (5 pcs)", "Regular", 1595);
        bbq[bbqCount++] = new BBQItem("Behari Kabab", "Regular", 1795);
        bbq[bbqCount++] = new BBQItem("Mutton Kabab (5 pcs)", "Regular", 2395);
        bbq[bbqCount++] = new BBQItem("Lahori / Peshawari Chops", "Regular", 2995);

        // Platter
        platter[platterCount++] = new MenuItem("Starter Platter", "For2", 1845);
        platter[platterCount++] = new MenuItem("Starter Platter", "Family", 3295);
        platter[platterCount++] = new MenuItem("BBQ Platter", "For2", 5995);
        platter[platterCount++] = new MenuItem("BBQ Platter", "Family", 9997);
        platter[platterCount++] = new MenuItem("Kabab Platter", "For2", 2395);
        platter[platterCount++] = new MenuItem("Kabab Platter", "Family", 4495);
        platter[platterCount++] = new MenuItem("Vegetarian Platter", "Family", 4995);
        platter[platterCount++] = new MenuItem("Burger Platter", "Family", 4995);
        platter[platterCount++] = new MenuItem("Dine360 Super Platter", "Family", 19995);

        // Handi
        handi[handiCount++] = new MenuItem("Chicken Boneless Handi", "Half", 1495);
        handi[handiCount++] = new MenuItem("Chicken Boneless Handi", "Full", 2595);
        handi[handiCount++] = new MenuItem("Chicken Makhni Handi", "Half", 1495);
        handi[handiCount++] = new MenuItem("Chicken Makhni Handi", "Full", 2595);
        handi[handiCount++] = new MenuItem("Chicken Achari Handi", "Half", 1495);
        handi[handiCount++] = new MenuItem("Chicken Achari Handi", "Full", 2595);
        handi[handiCount++] = new MenuItem("Chicken Cheese Handi", "Half", 1595);
        handi[handiCount++] = new MenuItem("Chicken Cheese Handi", "Full", 2795);
        handi[handiCount++] = new MenuItem("Mutton Boneless Handi", "Half", 2295);
        handi[handiCount++] = new MenuItem("Mutton Boneless Handi", "Full", 4095);
        handi[handiCount++] = new MenuItem("Mutton Makhni Handi", "Half", 2295);
        handi[handiCount++] = new MenuItem("Mutton Makhni Handi", "Full", 4095);
        handi[handiCount++] = new MenuItem("Mutton Achari Handi", "Half", 2295);
        handi[handiCount++] = new MenuItem("Mutton Achari Handi", "Full", 4095);

        // Dessert
        dessert[dessertCount++] = new DessertItem("Dine360 Dream Cake", "Regular", 795);
        dessert[dessertCount++] = new DessertItem("Mughlai Milk Cake", "Regular", 795);
        dessert[dessertCount++] = new DessertItem("Gulab Jamun", "Regular", 395);
        dessert[dessertCount++] = new DessertItem("Ras Malai", "Regular", 445);
        dessert[dessertCount++] = new DessertItem("Gajjar Ka Halwa", "Regular", 495);
        dessert[dessertCount++] = new DessertItem("Nutella Naan", "Regular", 445);
        dessert[dessertCount++] = new DessertItem("Oreo Sundae", "Regular", 395);
        dessert[dessertCount++] = new DessertItem("Biscoff Lotus Sundae", "Regular", 495);

        // Nashta
        nashta[nashtaCount++] = new MenuItem("Halwa Puri Thaal", "Regular", 695);
        nashta[nashtaCount++] = new MenuItem("Lassi", "Regular", 395);
        nashta[nashtaCount++] = new MenuItem("Karak Chai", "Regular", 295);
        nashta[nashtaCount++] = new MenuItem("Sarson Ka Saag", "Regular", 1195);
        nashta[nashtaCount++] = new MenuItem("Mutton Paya", "Regular", 1195);
        nashta[nashtaCount++] = new MenuItem("Beef Nehari", "Regular", 1695);
        nashta[nashtaCount++] = new MenuItem("Chicken Haleem", "Regular", 795);
    }

    ~MenuManager()
    {
        for (int i = 0; i < starterCount; i++)
            delete starters[i];

        for (int i = 0; i < karahiCount; i++) 
            delete karahi[i];

        for (int i = 0; i < bbqCount; i++) 
            delete bbq[i];

        for (int i = 0; i < platterCount; i++) 
            delete platter[i];

        for (int i = 0; i < handiCount; i++) 
            delete handi[i];

        for (int i = 0; i < dessertCount; i++) 
            delete dessert[i];

        for (int i = 0; i < nashtaCount; i++) 
            delete nashta[i];
    }

    MenuItem** getCategory(int index, int& outCount)
    {
        switch (index)
        {
        case 0: 
            outCount = starterCount; 
            return starters;
        case 1: 
            outCount = karahiCount;  
            return karahi;
        case 2:
            outCount = bbqCount; 
            return bbq;
        case 3:
            outCount = platterCount; 
            return platter;
        case 4:
            outCount = handiCount;  
            return handi;
        case 5:
            outCount = dessertCount;
            return dessert;
        case 6: 
            outCount = nashtaCount; 
            return nashta;
        default: 
            outCount = 0;
            return nullptr;
        }
    }
};

// UI MANAGER 
class UIManager
{
private:
    MenuManager* menus = nullptr;
    bool showPopup = false;
    string popupText = "";

public:
    Font font;
    Texture2D menuIcon;
    Texture2D crossIcon;

    UIManager(MenuManager* m = nullptr) : menus(m) {}

    void setMenuManager(MenuManager* m)
    {
        menus = m; 
    }

    // Asset management
    void loadAssets()
    {
        font = LoadFontEx("font.ttf", 32, 0, 0);
        if (font.texture.id == 0) font = GetFontDefault();
        menuIcon = LoadTexture("hamburgericon.png");
        crossIcon = LoadTexture("cross.png");
    }

    void unloadAssets()
    {
        UnloadFont(font);
        UnloadTexture(menuIcon);
        UnloadTexture(crossIcon);
    }


    bool DrawFancyButton(Rectangle rect, const char* text) const
    {
        Vector2 mouse = GetMousePosition();
        bool    hover = CheckCollisionPointRec(mouse, rect);

        // Drop shadow
        DrawRectangleRounded({ rect.x + 3, rect.y + 3, rect.width, rect.height }, 0.3f, 10, Color{ 0, 0, 0, 80 });

        DrawRectangleRounded(rect, 0.3f, 10, hover ? Theme::HOVER : Theme::PRIMARY);
        DrawRectangleRoundedLines(rect, 0.3f, 10, Color{ 255, 255, 255, 60 });

        Vector2 ts = MeasureTextEx(font, text, 20, 2);
        DrawTextEx(font, text,  { rect.x + (rect.width - ts.x) / 2,  rect.y + (rect.height - ts.y) / 2 },  20, 2, WHITE);

        return hover;
    }

    void triggerPopup(const string& msg)
    {
        showPopup = true;
        popupText = msg;
    }

    void DrawPopup()
    {
        if (!showPopup) 
            return;

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 0, 0, 0, 150 });

        Rectangle box = {
            (float)(GetScreenWidth() / 2 - 200),
            (float)(GetScreenHeight() / 2 - 100),
            400, 200
        };
        DrawRectangleRounded(box, 0.2f, 10, WHITE);

        Vector2 ts = MeasureTextEx(font, popupText.c_str(), 24, 2);
        DrawTextEx(font, popupText.c_str(),  { box.x + (box.width - ts.x) / 2, box.y + 50 },  24, 2, BLACK);

        Rectangle okBtn = { box.x + 150, box.y + 120, 100, 40 };

        DrawRectangleRounded(okBtn, 0.3f, 10, Theme::PRIMARY);
        DrawTextEx(font, "OK", { okBtn.x + 30, okBtn.y + 10 }, 20, 2, WHITE);

        if (CheckCollisionPointRec(GetMousePosition(), okBtn) &&  IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            showPopup = false;
    }

    void DrawScrollbar(int x, int y, int h, float scroll, float contentH) const
    {
        if (contentH <= h)
            return;
        DrawRectangle(x, y, 6, h, Color{ 60, 60, 60, 255 });
        float barH = (float)h * ((float)h / contentH);
        float barY = (float)y + (scroll / contentH) * (float)h;
        DrawRectangle(x, (int)barY, 6, (int)barH, Theme::PRIMARY);
    }

    void DrawHeader(const char* title, const char* subtitle = nullptr) const
    {
        DrawRectangle(0, 0, Layout::SCREEN_W, 120, Theme::PRIMARY);
        Vector2 ts = MeasureTextEx(font, title, 42, 2);
        DrawTextEx(font, title,  { (Layout::SCREEN_W - ts.x) / 2.0f, 40 }, 42, 2, WHITE);
        if (subtitle)
        {
            Vector2 ss = MeasureTextEx(font, subtitle, 20, 2);
            DrawTextEx(font, subtitle,  { (Layout::SCREEN_W - ss.x) / 2.0f, 85 }, 20, 2, Fade(WHITE, 0.9f));
        }
    }

    int DrawCartRows(const Cart& cart, int panelX, int panelW,
        int panelY, int rowH, int scissorH, int scrollY) const
    {
        int drawY = panelY + 8 - scrollY;
        for (int i = 0; i < cart.getCount(); i++)
        {
            DrawRectangle(panelX, drawY - 2, panelW, rowH - 4, (i % 2 == 0) ? Theme::CARD : Color{ 240, 235, 220, 255 });

            DrawTextEx(font,  TextFormat("%s x%d   -   Rs. %d", cart.getItem(i)->getName().c_str(),
                    cart.getQty(i), cart.getItem(i)->getPrice() * cart.getQty(i)), { (float)(panelX + 15), (float)drawY }, 22, 2, Theme::TEXT);

            drawY += rowH;
        }
        return drawY;
    }

    void DrawInputField(const string& label, const string& value, Rectangle box, bool active, const char* placeholder) const
    {
        DrawTextEx(font, label.c_str(),  { box.x, box.y - 25 }, 20, 2, Theme::TEXT);
        DrawRectangleRounded(box, 0.2f, 6, WHITE);
        DrawRectangleLinesEx(box, 2, active ? Theme::PRIMARY : GRAY);

        if (value.empty())
            DrawTextEx(font, placeholder, { box.x + 10, box.y + 10 }, 18, 2, GRAY);
        else
            DrawTextEx(font, value.c_str(), { box.x + 10, box.y + 10 }, 20, 2, BLACK);

        if (active)
        {
            bool showCursor = ((int)(GetTime() * 2) % 2) == 0;
            if (showCursor)
            {
                float tw = MeasureTextEx(font, value.c_str(), 20, 2).x;
                DrawTextEx(font, "|", { box.x + 10 + tw + 2, box.y + 10 }, 20, 2, BLACK);
            }
        }
    }

    void DrawMenuTable(MenuItem** items, int count, int startX, int startY, Cart& cart, StockManager& stock, float menuScroll)
    {
        const int colItem = startX;
        const int colSize = startX + 380;
        const int colPrice = startX + 500;
        const int colAdd = startX + 620;

        // Column headers
        DrawTextEx(font, "Item", { (float)colItem,  (float)(startY - 30) }, 22, 2, Theme::TEXT);
        DrawTextEx(font, "Size", { (float)colSize,  (float)(startY - 30) }, 22, 2, Theme::TEXT);
        DrawTextEx(font, "Price", { (float)colPrice, (float)(startY - 30) }, 22, 2, Theme::TEXT);
        DrawTextEx(font, "Add", { (float)colAdd,   (float)(startY - 30) }, 22, 2, Theme::TEXT);
        DrawLine(startX, startY - 5, startX + 650, startY - 5, BLACK);

        const int scissorX = 350, scissorY = 160, scissorW = 750, scissorH = 600;
        BeginScissorMode(scissorX, scissorY, scissorW, scissorH);

        for (int i = 0; i < count; i++)
        {
            string name = items[i]->getName();
            string size = items[i]->getSize();

            string fullName = name + " (" + size + ")";
            int inStock = stock.getStock(fullName);
            int already = cart.getQuantity(name, size);       
            bool outOfStock = (inStock <= 0 || already >= inStock);

            int y = startY + i * 40 - (int)menuScroll;
            if (y + 40 < scissorY || y > scissorY + scissorH)
                continue;

            DrawRectangle(startX - 10, y - 5, 720, 35,  (i % 2 == 0) ? Theme::CARD : Theme::SECONDARY);

            DrawTextEx(font, name.c_str(),
                { (float)colItem,  (float)y }, 20, 2, Theme::TEXT);
            DrawTextEx(font, size.c_str(),
                { (float)colSize,  (float)y }, 20, 2, BLACK);
            DrawTextEx(font, TextFormat("Rs. %d", items[i]->getPrice()),
                { (float)colPrice, (float)y }, 20, 2, BLACK);

            // Add button with hover scale
            Rectangle plusBtn = { (float)colAdd, (float)y, 30, 30 };
            bool plusHover = CheckCollisionPointRec(GetMousePosition(), plusBtn);
            float scale = plusHover ? 1.15f : 1.0f;
            Rectangle scaledBtn = {
                plusBtn.x - (plusBtn.width * (scale - 1) / 2),
                plusBtn.y - (plusBtn.height * (scale - 1) / 2),
                plusBtn.width * scale,
                plusBtn.height * scale
            };

            if (plusHover)
            {
                DrawRectangleRounded(
                    { scaledBtn.x - 2, scaledBtn.y - 2,
                      scaledBtn.width + 4, scaledBtn.height + 4 },
                    0.5f, 8, Color{ 255, 215, 100, 80 });
            }

            if (outOfStock)
            {
                DrawRectangleRounded(scaledBtn, 0.5f, 8, GRAY);
                Vector2 xs = MeasureTextEx(font, "X", 20, 2);
                DrawTextEx(font, "X",
                    { scaledBtn.x + (scaledBtn.width - xs.x) / 2,
                      scaledBtn.y + (scaledBtn.height - xs.y) / 2 },
                    20, 2, RED);
            }
            else
            {
                DrawRectangleRounded(scaledBtn, 0.5f, 8,
                    plusHover ? Theme::HOVER : Theme::PRIMARY);
                Vector2 ps = MeasureTextEx(font, "+", 20, 2);
                DrawTextEx(font, "+",
                    { scaledBtn.x + (scaledBtn.width - ps.x) / 2,
                      scaledBtn.y + (scaledBtn.height - ps.y) / 2 },
                    20, 2, WHITE);
            }

            if (plusHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (outOfStock)
                    triggerPopup("Item is out of stock!");
                else
                    cart.add(items[i]);
            }

            if (plusHover)
                SetMouseCursor(outOfStock ? MOUSE_CURSOR_NOT_ALLOWED
                    : MOUSE_CURSOR_POINTING_HAND);
        }

        EndScissorMode();
        DrawPopup();
    }
};

void applyScroll(float& scroll, float& targetScroll, float wheelDelta, int maxScroll)
{
    targetScroll -= wheelDelta * 40.0f;
    if (targetScroll < 0)    
        targetScroll = 0;
    if (targetScroll > maxScroll) 
        targetScroll = (float)maxScroll;
    scroll += (targetScroll - scroll) * 0.15f;
    if (scroll < 0)  
        scroll = 0;
    if (scroll > maxScroll) 
        scroll = (float)maxScroll;
}

inline int calcMaxScroll(int contentH, int viewH)
{
    int m = contentH - viewH;
    return m > 0 ? m : 0;
}