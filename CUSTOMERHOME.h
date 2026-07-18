#ifndef CUSTOMERHOME_H
#define CUSTOMERHOME_H

#include "SCREENS.h"
#include "CUSTOMER.h"

class CustomerHome {
private:
    Customer* customer;

    // notification toast
    char message[128];
    float messageTimer;
    bool  messageVisible;

    // hover states
    bool logoutHover;
    bool placeOrderHover;
    bool reservationsHover;

    // rectangles
    Rectangle logoutBtn;
    Rectangle placeOrderBtn;
    Rectangle reservationsBtn;
    //n:
    bool orderFoodRequested;
    bool reservationRequested;
    bool orderFoodHover;
    bool reservationHover;
    Rectangle orderFoodBtn;
    Rectangle reservationBtn;

public:
    CustomerHome();
    void setCustomer(Customer* c);
    Screen update();
    void draw();
    bool isReservationRequested();
    bool isOrderFoodRequested();
};

#endif