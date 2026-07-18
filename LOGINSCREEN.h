#ifndef LOGINSCREEN_h
#define LOGINSCREEN_h
#include "SCREENS.h"
class LoginScreen {
	char usernameBuffer[64];
	char passwordBuffer[64];
	int usernameLen;
	int passwordLen;
	bool usernameActive; //checks activity for respective buffer input
	bool passwordActive;
	bool showPassword;
	bool isStaff;
	char message[128];
	bool isError;
	bool loginHover;//HOVERS: check if the mouse is over that button's rectangle 
	bool signupHover;
	bool customerToggleHover;
	bool staffToggleHover;
	float cursorTimer;
	bool cursorVisible;
	Rectangle usernameBox;
	Rectangle passwordBox;
	Rectangle loginBtn;
	Rectangle signupBtn;
	Rectangle customerToggle;
	Rectangle staffToggle;
	Auth auth;//to use ayth class functions
	int usernameCursorPos;
	int passwordCursorPos;
public:
	LoginScreen();// constructor, sets up all initial values 
	Screen update(Customer& loggedCustomer, Staff& loggedStaff);//runs every frame, handles all input and logic, returns the next screen
	void draw();
};
#endif