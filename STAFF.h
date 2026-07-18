#ifndef STAFF_h
#define STAFF_h
#include "USER.h"
class Staff: public User {
private:
	string designation;
	string permission;
	void setPermission(string perm);
public:
	Staff();
	Staff(string n, string u, string p, string des,string perm);
	void setDesignation(string des);
	string getDesignation()const;
	string getPermission()const;
	void displayDashboard() override;
	~Staff();
};
#endif