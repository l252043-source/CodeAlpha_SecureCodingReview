#pragma once
#include "Reservation.h"
class Event : public Reservation {
private:
    char  eventType[32];
    int   packageTier;
    bool  hasDecor;
    bool  hasSpecialMenu;
    bool  hasDJ;
    float totalCost;
public:
    //Constructor
    Event();
    Event(int id, const char* tType, const char* d, const char* t, int ps, const char* evType, int pkg, bool deco, bool menu, bool dj);
    //Destructor
    ~Event();
    //Getters
    const char* getEventType() const;
    int getPackage() const;
    bool getDecor() const;
    bool getSpecialMenu() const;
    bool getDJ() const;
    float getTotalCost() const;
    //Setters
    void setEventType(const char* et);
    void setPackage(int pkg);
    void setDecor(bool d);
    void setSpecialMenu(bool m);
    void setDJ(bool dj);
    //Calculation
    void calculateCost();
    //Display
    void display() const override;
};