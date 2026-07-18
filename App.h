#pragma once
#include <raylib.h>
#include "Theme.h"
#include "Managers.h"
#include "menuScreens.h"

class App
{
private:
    AppState appState;
    UIManager uiManager;
    MenuManager menuManager;
    Screen* screens[6];

public:
    App()
    {
        appState.loadTables();
        appState.validateTableTimes();
        appState.stock.loadStock();

        uiManager.setMenuManager(&menuManager);

        screens[0] = new MenuScreen(appState, uiManager, menuManager);
        screens[1] = new OrderScreen(appState, uiManager);
        screens[2] = new TableScreen(appState, uiManager);
        screens[3] = new PaymentScreen(appState, uiManager);
        screens[4] = new DeliveryScreen(appState, uiManager);
        screens[5] = new BillScreen(appState, uiManager);
    }

    ~App()
    {
        for (int i = 0; i < 6; i++)
            delete screens[i];
    }

    void run()
    {
        InitWindow(Layout::SCREEN_W, Layout::SCREEN_H, "Dine360");
        SetTargetFPS(60);
        uiManager.loadAssets();

        while (!WindowShouldClose())
        {
            int idx = appState.currentScreen;

            if (idx >= 0 && idx < 6)
                screens[idx]->update();

            BeginDrawing();
            ClearBackground(Theme::BACKGROUND);

            if (idx >= 0 && idx < 6)
                screens[idx]->draw();

            EndDrawing();
        }

        uiManager.unloadAssets();
        CloseWindow();
    }
};