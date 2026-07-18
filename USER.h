#ifndef USER_h
#define USER_h
#include<iostream>
#include<string>
using namespace std;
class User {
protected://protected data members, so only child classes can access
	string name;
	string username;
	string password;
	string role;
public:
	//default constructor:
	User();
	//parameterized constructor:
	User(string n, string u, string p, string r);
	//getter/setters:
	void setName(string n);
	void setUsername(string u);
	void setPassword(string p);
	void setRole(string r);
	string getName()const;
	string getUsername()const;
	string getPassword()const;
	string getRole()const;
	//function:
	virtual void displayDashboard()=0;
	//destructor:
	virtual ~User()=0;
};
#endif;

