#define _CRT_SECURE_NO_WARNINGS
#include "Event.h"
#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;
static const int package_fee[4] = { 5000, 8000, 25000, 20000 };
static const char* package_name[4] = { "Basic", "Business", "Premium", "Corporate" };
//Default constructor
Event::Event() : Reservation() {
    strcpy(eventType, "");
    packageTier = 0;
    hasDecor = false;
    hasSpecialMenu = false;
    hasDJ = false;
    totalCost = 0.0f;
}

//Parameterized constructor
Event::Event(int id, const char* tType, const char* d, const char* t, int ps, const char* evType, int pkg, bool deco, bool menu, bool dj) : Reservation(id, tType, d, t, ps) {
    setEventType(evType);
    setPackage(pkg);
    hasDecor = deco;
    hasSpecialMenu = menu;
    hasDJ = dj;
    calculateCost();
}
//Destructor
Event::~Event() {}
//Getters
const char* Event::getEventType() const {
    return eventType;
}
int Event::getPackage() const {
    return packageTier;
}
bool Event::getDecor() const {
    return hasDecor;
}
bool Event::getSpecialMenu() const {
    return hasSpecialMenu;
}
bool Event::getDJ() const {
    return hasDJ;
}
float Event::getTotalCost() const {
    return totalCost;
}
//Setters
void Event::setEventType(const char* et)
{
    strncpy(eventType, et, 31); eventType[31] = '\0';
}
void Event::setPackage(int pkg) {
    if (pkg < 0) pkg = 0;
    if (pkg > 3) pkg = 3;
    packageTier = pkg;
}
void Event::setDecor(bool d) {
    hasDecor = d;
}
void Event::setSpecialMenu(bool m) {
    hasSpecialMenu = m;
}
void Event::setDJ(bool dj) {
    hasDJ = dj;
}
//Calculate total cost
void Event::calculateCost() {
    float cost = (float)package_fee[packageTier];
    cost += 500.0f * getPartySize();
    if (hasDecor) {
        cost += 5000.0f;
    }
    if (hasSpecialMenu) {
        cost += 3000.0f;
    }
    if (hasDJ) {
        cost += 8000.0f;
    }
    totalCost = cost;
}
//Display
void Event::display() const {
    Reservation::display();
    cout << "Event Type: " << eventType << endl;
    cout << "Package: " << package_name[packageTier] << endl;
    cout << "Decor: " << (hasDecor ? "Yes" : "No") << endl;
    cout << "Menu: " << (hasSpecialMenu ? "Yes" : "No") << endl;
    cout << "DJ: " << (hasDJ ? "Yes" : "No") << endl;
    cout << "Total Cost : Rs." << (int)totalCost << endl;
}