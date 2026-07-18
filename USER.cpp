#include "USER.h"
User::User() {
	name = "";
	username = "";
	password = "";
	role = "";
}
User::User(string n, string u, string p, string r) {
	name = n;
	username = u;
	password = p;
	role = r;
}
void User::setName(string n) { name = n; }
void User::setUsername(string u) {username = u;}
void User::setPassword(string p) { password = p; }
void User::setRole(string r) { role = r; }
string User::getName()const { return name; }
string User::getUsername()const { return username;}
string User::getPassword()const {	return password;}
string User::getRole()const {return role;}

void User::displayDashboard() 
{	cout << "     " << endl; }

User::~User() {}