// src/kernel/shell.h
#ifndef SHELL_H
#define SHELL_H

class Shell {
public:
    Shell();
    void run();
    
private:
    void processCommand(char key);
    void printHelp();
    void printAbout();
    void printEchoMode();
    void toggleEchoMode();
    void clearScreen();
    void reboot();
    void printMemoryInfo();
    void printKeyboardInfo();
    void printDiskInfo();
    void printTaskInfo();
    void printConsoleInfo();
    void printSyscallInfo();
    void printNextSteps();
    void printProcessInfo();
    void nextColor();
    void printPositionDemo();
    
    bool echoMode;
    int currentColorIndex;
    bool running;
};

#endif