// src/kernel/keyboard.cpp
#include "keyboard.h"
#include "console.h"

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

bool Keyboard::isShiftPressed = false;
bool Keyboard::isCapsLock = false;

void Keyboard::init() {
    isShiftPressed = false;
    isCapsLock = false;
}

bool Keyboard::keyPressed() {
    uint8_t status = inb(0x64);
    return status & 0x01;
}

uint8_t Keyboard::readScancode() {
    return inb(0x60);
}

char Keyboard::getChar() {
    while (true) {
        char c = getCharNonBlock();
        if (c != 0) {
            return c;
        }
        for (int i = 0; i < 100000; i++) {
            asm volatile ("nop");
        }
    }
}

char Keyboard::getCharNonBlock() {
    if (!keyPressed()) {
        return 0;
    }
    
    uint8_t scancode = readScancode();
    
    // Игнорируем release коды (>= 0x80)
    if (scancode & 0x80) {
        return 0;
    }
    
    // ВРЕМЕННО: просто возвращаем scancode как символ
    // (для отладки — выводим код клавиши)
    return scancode;
}

char Keyboard::scancodeToAscii(uint8_t scancode) {
    // Временная заглушка
    return scancode;
}