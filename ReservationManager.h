#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Reservation.h"
#include "Event.h"

const int MAX_RES = 100;
const int MAX_EVT = 100;
const int MAX_TWY = 100;

// -------------------------------------------------------
//  TakeawayOrder
// -------------------------------------------------------
struct TakeawayOrder
{
    int   orderID;
    char  customerName[64];
    char  phone[32];
    int   qty[8];
    float totalCost;
    char  status[16];
};

// -------------------------------------------------------
//  ReservationManager
//  Muhammad Muneeb (25L-2043)
//  Team 4  BCY-2A  OOP Project  FAST-NUCES
// -------------------------------------------------------
class ReservationManager
{
private:
    // ---- Core data arrays ----
    Reservation   reservations[MAX_RES];
    Event         events[MAX_EVT];
    TakeawayOrder twOrders[MAX_TWY];

    int resCount = 0;
    int evtCount = 0;
    int twCount = 0;
    int nextID = 1001;

    // ---- Table state ----
    int tableOccupied[12];          // 0 = free, 1 = occupied
    int reservation_tableIDX[MAX_RES]; // which table index each reservation used

    // ---- Floor plan state ----
    int selectedTable = -1;
    int partySize = 2;

    // ---- Advance booking form (Screen 1) ----
    char booking_tableType[32];
    char booking_date[64];   bool booking_date_on;   int booking_date_tick;
    char booking_time[64];   bool booking_time_on;   int booking_time_tick;
    char booking_note[128];  bool booking_note_on;   int booking_note_tick;
    int  booking_pkg;
    char booking_error[128];

    // ---- Event form (Screen 2) ----
    char event_date[64];     bool ev_date_on;    int ev_date_tick;
    char event_time[64];     bool ev_time_on;    int ev_time_tick;
    char event_guests[16];   bool ev_guests_on;  int ev_guests_tick;
    char event_error[128];
    int  ev_type;
    int  ev_pkg;
    bool ev_deco;
    bool ev_menu;
    bool event_sound;        // DJ / Sound add-on

    // ---- Search by ID (Screen 7) ----
    // search_resIDX / search_evtIDX:
    //   -2 = idle (no search run yet)
    //   -1 = searched but not found
    //   >= 0 = index of matching record
    char search_input[16];
    bool search_input_on;
    int  search_input_tick;
    int  search_resIDX;
    int  search_evtIDX;
    char search_error[128];
    int  search_tab;                  // 0 = Reservation  |  1 = Event Booking
    int  search_cancelConfirmRes;
    int  search_cancelConfirmEvt;

    // ---- Scroll positions ----
    int resScroll = 0;
    int evtScroll = 0;
    int twScroll = 0;

    //2-step cancel confirmation (list screens)
    int cancelConfirmRes = -1;
    int cancelConfirmEvt = -1;
    int cancelConfirmTwy = -1;

    // ---- Private helpers ----
    const char* tableName(int idx) const;
    const char* tableTypeName(int idx) const;

    // ---- Screen draw methods ----
    void drawModeSelect();   // screen 5  — startup hub
    void drawFloorPlan();    // screen 0  — table map
    void drawBookForm();     // screen 1  — advance booking
    void drawWalkIn();       // screen 6  — instant walk-in
    void drawEventForm();    // screen 2  — event booking
    void drawResList();      // screen 3  — all reservations
    void drawEvtList();      // screen 4  — all events
    void drawSearchByID();   // screen 7  — search by ID

    //Cancel helpers 
    void cancelReservation(int idx);
    void cancelEvent(int idx);

public:
    
    int currentScreen = 5;

    ReservationManager();
    ~ReservationManager();

    void update();   

    void addReservation(int tableIdx, const char* tType,
        const char* date, const char* time,
        int guests, int pkg);

    void addEvent(const char* eType, const char* date, const char* time,
        int guests, int pkg, bool deco, bool menu, bool dj);

    int  getResCount() const;
    int  getEvtCount() const;
    int  getOccupied() const;
    bool isSlotTaken(int tableIdx, const char* date, const char* time) const;
};