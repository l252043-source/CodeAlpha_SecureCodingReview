#ifndef CUSTOMER_h
#define	CUSTOMER_h
#include "USER.h"
class Customer : public User {
private:
	string address;
	string orderHistory;
public:
	//default constructor:
	Customer();
	//parameterized constructor:
	Customer(string n, string u, string p, string a);
	//getter/setter:
	void setAddress(string a);
	void setOrderHistory(string o);
	string getAddress()const;
	string getOrderHistory()const;
	//function:
	void displayDashboard() override;
	//destructor:
	~Customer() override;
};
#endif