#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

class Keyboard {
public:
    static void init();
    static char getChar();
    static char getCharNonBlock();
    static bool keyPressed();
    
private:
    static char scancodeToAscii(uint8_t scancode);
    static uint8_t readScancode();
    static bool isShiftPressed;
    static bool isCapsLock;
};

#endif