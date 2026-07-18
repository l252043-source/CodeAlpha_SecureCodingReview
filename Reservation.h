#pragma once
#include <cstring>
#include <cstdio>
class Reservation {
private:
    int  resID;
    char tableType[32];
    char date[32];
    char time[32];
    int  partySize;
    char status[16];

public:
    //Constructor
    Reservation();
    Reservation(int id, const char* table_type, const char* d, const char* t, int ps);
    //Destructor
    virtual ~Reservation();
    //Getters
    int getID() const;
    const char* getTableType() const;
    const char* getDate() const;
    const char* getTime() const;
    int getPartySize() const;
    const char* getStatus() const;
    //Setters
    void setDate(const char* d);
    void setTime(const char* t);
    void setPartySize(int ps);
    void setStatus(const char* s);
    //Actions
    void confirm();
    void cancel();
    //Display
    virtual void display() const;
};