#ifndef SIGNUPSCREEN_H
#define SIGNUPSCREEN_H

#include "Screens.h"

class SignupScreen {
private:
    char nameBuffer[64];
    char usernameBuffer[64];
    char passwordBuffer[64];
    char confirmBuffer[64];
    char extraBuffer[64];
    char adminBuffer[64];

    int nameLen;
    int usernameLen;
    int passwordLen;
    int confirmLen;
    int extraLen;
    int adminLen;

    // cursor positions for each field (index 1-6)
    int cursorPos[7];

    // 0=none 1=name 2=username 3=password 4=confirm 5=extra 6=admin
    int activeField;

    bool isStaff;
    bool showPassword;
    bool showConfirm;

    bool signupBtnHover;
    bool backBtnHover;
    bool customerToggleHover;
    bool staffToggleHover;

    float cursorTimer;
    bool cursorVisible;

    char message[128];
    bool isError;

    float successTimer;
    bool successTriggered;

    Rectangle customerToggle;
    Rectangle staffToggle;
    Rectangle nameBox;
    Rectangle usernameBox;
    Rectangle passwordBox;
    Rectangle confirmBox;
    Rectangle extraBox;
    Rectangle adminBox;
    Rectangle signupBtn;
    Rectangle backBtn;

    Auth auth;

    int  getPasswordStrength();
    bool validateFields();
    void clearBuffer(char* buf, int& len);
    void typeIntoBuffer(char* buf, int& len, int fieldIndex);
    void setMessage(const char* msg, bool error);

public:
    SignupScreen();
    void reset();
    Screen update();
    void draw();
};

#endif