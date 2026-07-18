#include "CUSTOMER.h"
Customer::Customer():User() {
	address = "";
	orderHistory = "";
}
Customer::Customer(string n, string u, string p, string a) :User(n, u, p, "customer") {
	address = a;
	orderHistory = "";
}
void Customer::setAddress(string a) { address = a; }
void Customer::setOrderHistory(string o) { 
	if (orderHistory == "")
		orderHistory = o;
	else
		orderHistory = orderHistory + "," + o;
}
string Customer::getAddress()const { return address; }
string Customer::getOrderHistory()const { return orderHistory; }
void Customer::displayDashboard()  {
	cout << "Welcome" << " " << name << endl;
	cout << "1. View Menu" << endl;
	cout << "2. Place Order" << endl;
	cout << "3. View Cart" << endl;
	cout << "4. Make Reservation" << endl;
	cout << "5. View Order History" << endl;
	cout << "6. Logout"<<endl;
	cout << "Enter your choice:"<<" ";
}
Customer::~Customer() {}