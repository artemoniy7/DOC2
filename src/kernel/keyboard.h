// src/kernel/keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>

class Keyboard {
public:
    static void init();
    static char getChar();      // Блокирующий вызов
    static char getCharNonBlock(); // Неблокирующий вызов
    static bool keyPressed();
    
private:
    static char scancodeToAscii(uint8_t scancode);
    static uint8_t readScancode();
    static bool isShiftPressed;
    static bool isCapsLock;
};

#endif