#define _CRT_SECURE_NO_WARNINGS
#include "Reservation.h"
#include<iostream>
#include <cstdio>
#include <cstring>
using namespace std;
//Default constructor
Reservation::Reservation() {
    resID = 0;
    partySize = 0;
    strcpy(tableType, "");
    strcpy(date, "");
    strcpy(time, "");
    strcpy(status, "Pending");
}
//Parameterized constructor
Reservation::Reservation(int id, const char* table_type, const char* d, const char* t, int ps) {
    resID = id;
    strncpy(tableType, table_type, 31);
    tableType[31] = '\0';
    strncpy(date, d, 31);
    date[31] = '\0';
    strncpy(time, t, 31);
    time[31] = '\0';
    setPartySize(ps);
    strcpy(status, "Confirmed");
}
//Destructor
Reservation::~Reservation() {}
//Getters 
int Reservation::getID() const {
    return resID;
}
const char* Reservation::getTableType() const {
    return tableType;
}
const char* Reservation::getDate() const {
    return date;
}
const char* Reservation::getTime() const {
    return time;
}
int Reservation::getPartySize() const {
    return partySize;
}
const char* Reservation::getStatus() const {
    return status;
}
//Setters
void Reservation::setDate(const char* d) {
    strncpy(date, d, 31);
    date[31] = '\0';
}

void Reservation::setTime(const char* t) {
    strncpy(time, t, 31);
    time[31] = '\0';
}
void Reservation::setPartySize(int ps) {

    if (ps < 1) ps = 1;
    if (ps > 50) ps = 50;
    partySize = ps;
}
void Reservation::setStatus(const char* s) {
    strncpy(status, s, 15);
    status[15] = '\0';
}
//Actions
void Reservation::confirm() {
    strcpy(status, "Confirmed");
}
void Reservation::cancel() {
    strcpy(status, "Cancelled");
}
//Display
void Reservation::display() const {
    cout << "--- Reservation #" << resID << " ---" << endl;
    cout << "Table Type: " << tableType << endl;
    cout << "Date: " << date << endl;
    cout << "Time: " << time << endl;
    cout << "Party Size: " << partySize << endl;
    cout << "Status: " << status << endl;
}