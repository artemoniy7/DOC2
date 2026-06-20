// src/kernel/keyboard.cpp
#include "keyboard.h"
#include "console.h"

// Вспомогательные функции для работы с портами
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

bool Keyboard::isShiftPressed = false;
bool Keyboard::isCapsLock = false;

// Таблица scancode -> ASCII (без Shift)
static const char scancodeTable[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,  'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  '\\','z','x','c','v','b','n','m',',','.','/',
    0,  '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7','8','9','-','4','5','6','+','1',
    '2','3','0','.'
};

// Таблица для Shift
static const char scancodeShiftTable[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,  'A','S','D','F','G','H','J','K','L',':','"','~',
    0,  '|','Z','X','C','V','B','N','M','<','>','?',
    0,  '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7','8','9','-','4','5','6','+','1',
    '2','3','0','.'
};

void Keyboard::init() {
    isShiftPressed = false;
    isCapsLock = false;
}

bool Keyboard::keyPressed() {
    // Проверяем, есть ли данные в буфере клавиатуры
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
        // Маленькая пауза
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
    
    // Обработка специальных клавиш
    if (scancode == 0x2A || scancode == 0x36) { // Left/Right Shift pressed
        isShiftPressed = true;
        return 0;
    }
    if (scancode == 0xAA || scancode == 0xB6) { // Left/Right Shift released
        isShiftPressed = false;
        return 0;
    }
    if (scancode == 0x3A) { // Caps Lock
        isCapsLock = !isCapsLock;
        return 0;
    }
    
    // Игнорируем release коды (>= 0x80)
    if (scancode & 0x80) {
        return 0;
    }
    
    return scancodeToAscii(scancode);
}

char Keyboard::scancodeToAscii(uint8_t scancode) {
    if (scancode >= 128) return 0;
    
    char c;
    if (isShiftPressed) {
        c = scancodeShiftTable[scancode];
    } else {
        c = scancodeTable[scancode];
    }
    
    // Обработка Caps Lock
    if (isCapsLock && c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
    } else if (isCapsLock && c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
    }
    
    return c;
}