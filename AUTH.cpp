#include"AUTH.h"
bool Auth::usernameExists(string username, string filename) {
	ifstream file(filename);
	string line;
	while (getline(file, line)) {
		stringstream ss(line);//reads before comma
		string n, u;
		getline(ss, n, ',');
		getline(ss, u, ',');
		if (u == username) {
			file.close();
			return true;
		}
	}
	file.close();
	return false;
}
void Auth::writeCustomer(string name, string username, string password, string address) {
	ofstream file("customer.txt",ios::app);
	file << name << "," << username << "," << password << "," << address << ","<<endl;
	file.close();
}
void Auth::writeStaff(string name, string username, string password, string designation, string permission) {
	ofstream file("staff.txt", ios::app);
	file << name << "," << username << "," << password << "," << designation << "," << permission<<","<<endl;
	file.close();
}
Customer Auth::readCustomer(string line) {
	stringstream ss(line);
	string n, u, p, a, o;
	getline(ss, n, ',');
	getline(ss, u, ',');
	getline(ss, p, ',');
	getline(ss, a, ',');
	getline(ss, o, ',');
	Customer c(n, u, p, a);
	return c;
}
Staff Auth::readStaff(string line) {
	stringstream ss(line);
	string n, u, p, des,perm;
	getline(ss, n, ',');
	getline(ss, u, ',');
	getline(ss, p, ',');
	getline(ss, des, ',');
	getline(ss, perm, ',');
	Staff s(n, u, p, des,perm);
	return s;
}
bool Auth::signup(string name, string username, string password, string role, string extra1, string code) {
	if (role == "customer") {
		if (usernameExists(username, "customer.txt"))
			return false;
		writeCustomer(name, username, password, extra1);
		return true;
	}
	else if (role == "staff") {
		if (code != "DINE360ADMIN") {
			return false;
		}
		if (usernameExists(username, "staff.txt"))
			return false;
		string permission = getPermission(extra1);
		writeStaff(name, username, password, extra1,permission);
		return true;
	}
	return false;
}
string Auth::login(string username, string password, string role, Customer& c, Staff& s) {
	if (role == "customer") {
		ifstream cFile("customer.txt");
		string line;
		while (getline(cFile, line)) {
			stringstream ss(line);
			string n, u, p;
			getline(ss, n, ',');
			getline(ss, u, ',');
			getline(ss, p, ',');
			if (u == username && p == password) {
				cFile.close();
				c = readCustomer(line);
				return "customer";
			}
		}
		cFile.close();
	}
	else if (role == "staff") {
		ifstream sFile("staff.txt");
		string line;
		while (getline(sFile, line)) {
			stringstream ss(line);
			string n, u, p;
			getline(ss, n, ',');
			getline(ss, u, ',');
			getline(ss, p, ',');
			if (u == username && p == password) {
				sFile.close();
				s = readStaff(line);
				return "staff";
			}
		}
		sFile.close();
	}
	return "FAILED";
}
string Auth::getPermission(string designation) {
	if (designation == "manager")
		return "all";
	else if (designation == "waiter")
		return "orders,reservations";
	else if (designation == "chef")
		return "orders,inventory";
	else if (designation == "cashier")
		return "orders,billing";
	else
		return "none";
}