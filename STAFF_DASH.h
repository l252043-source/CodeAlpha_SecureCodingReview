#ifndef STAFFDASHBOARD_H
#define STAFFDASHBOARD_H

#include "SCREENS.h"
#include "STAFF.h"
#include <fstream>
#include <sstream>

enum StaffPanel {
    SPANEL_INVENTORY,
    SPANEL_ORDERS,
    SPANEL_MENU
};

// single stock entry from Basit's stock.txt
struct StockEntry {
    char name[128];  // full name e.g. "Lahori Chicken Karahi (Full)"
    int  qty;
};

// single order from Basit's orders.txt
struct BasitOrder {
    char lines[10][128]; // item lines
    int  lineCount;
    char type[16];       // "1"=DineIn "2"=Takeaway "3"=Delivery
    char status[32];     // PENDING / CONFIRMED / DELIVERED
};

class StaffDashboard {
private:
    Staff* staff;
    StaffPanel activePanel;

    // ---- STOCK (Basit's stock.txt) ----
    StockEntry stock[100];
    int        stockCount;
    int        editingStock;    // index being edited, -1 = none
    char       editQtyBuf[16];
    int        editQtyLen;
    int        hoveredStock;
    int        confirmDeleteStock; // -1 = none
    bool       confirmDelYes;
    bool       confirmDelNo;

    // add new stock entry form
    bool showAddForm;
    char addName[128];
    char addQty[16];
    int  addNameLen;
    int  addQtyLen;
    int  activeAddField; // 1=name 2=qty
    int  addCursorPos[3];

    // ---- ORDERS (Basit's orders.txt) ----
    BasitOrder orders[100];
    int        orderCount;
    int        hoveredOrder;

    // ---- MANAGE MENU (edit name/qty inline) ----
    int  editMenuIndex;
    char editMenuName[128];
    char editMenuQty[16];
    int  editMenuNameLen;
    int  editMenuQtyLen;
    int  activeMenuField; // 1=name 2=qty
    int  menuEditCursorPos[3];

    // ---- SHARED ----
    char  notification[128];
    float notifTimer;
    bool  notifVisible;
    bool  notifIsError;
    bool  unsavedChanges;
    int   scrollOffset;
    float cursorTimer;
    bool  cursorVisible;

    // hover states
    bool logoutHover;
    bool invNavHover;
    bool ordNavHover;
    bool mnuNavHover;
    bool saveHover;
    bool addFormHover;
    bool confirmAddHover;
    bool cancelAddHover;
    bool refreshHover;

    // rectangles
    Rectangle logoutBtn;
    Rectangle invNavBtn;
    Rectangle ordNavBtn;
    Rectangle mnuNavBtn;
    Rectangle saveBtn;
    Rectangle addNewBtn;
    Rectangle refreshBtn;

    // helpers
    void loadStock();
    void saveStock();
    void loadOrders();
    void deleteStockEntry(int index);
    void addStockEntry();
    void showNotif(const char* msg, bool error);
    void clearAddForm();
    void typeField(char* buf, int& len,
        int* cursorArr, int fieldIdx, int maxLen);
    void drawField(Rectangle box, const char* label,
        char* buf, int len, int curPos,
        bool isActive, Font fontR);

    void drawInventoryPanel(float mainX, float mainW, int H);
    void drawOrdersPanel(float mainX, float mainW, int H);
    void drawManageMenuPanel(float mainX, float mainW, int H);
    void drawConfirmDeleteDialog(int W, int H);
    void drawNotification(int W, int H);
    void drawSidebar(float sideW, int H);

public:
    StaffDashboard();
    void setStaff(Staff* s);
    Screen update();
    void draw();
};

#endif