#ifndef AUTH_h
#define AUTH_h
#include"CUSTOMER.h"
#include"STAFF.h"
#include<fstream>
#include<sstream>//deals string before comma as individual
class Auth {
public:
	bool signup(string name, string username, string password, string role, string extra1, string code);
	string login(string username, string password, string role,Customer&c, Staff&s );
private:
	bool usernameExists(string username, string filename);
	void writeCustomer(string name, string username, string password, string address);
	void writeStaff(string name, string username, string password, string designation, string permission);
	Customer readCustomer(string line);
	Staff readStaff(string line);
	string getPermission(string designation);
};
#endif  AUTH_h
