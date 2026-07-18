#include"STAFF.h"
Staff::Staff(): User(){
	designation = "";
	permission = "";
}
Staff::Staff(string n, string u, string p, string des, string perm) :User(n, u, p, "staff") {
	designation = des;
	permission = perm;
}
void Staff::setDesignation(string des) {	designation = des;}
void Staff::setPermission(string perm) {permission = perm;}
string Staff::getDesignation()const {	return designation;}
string Staff::getPermission()const {return permission;}
void Staff::displayDashboard() {
    cout << "   ADMIN PANEL " << name << endl;
    cout << "   Role: " << designation << endl;
    cout << "1. Manage Menu / Inventory"<< endl;
    cout << "2. View All Orders"<< endl;
    cout << "3. Manage Reservations"<< endl;
    cout << "4. View All Customers"<< endl;
    cout << "5. Generate Reports"<< endl;
    cout << "6. Logout"<< endl;
    cout << "Enter your choice: ";
}
Staff::~Staff() {}

