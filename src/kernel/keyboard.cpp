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

// ПРАВИЛЬНАЯ таблица для Scancode Set 2 (используется в QEMU)
static const char scancodeTable[128] = {
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x00-0x07
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x08-0x0F
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x10-0x17
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x18-0x1F
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x20-0x27
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x28-0x2F
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x30-0x37
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x38-0x3F
    
    // Основные клавиши (Set 2)
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x40-0x47
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x48-0x4F
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x50-0x57
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x58-0x5F
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x60-0x67
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x68-0x6F
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x70-0x77
    0,   0,   0,   0,   0,   0,   0,   0,    // 0x78-0x7F
};

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
        // Небольшая задержка
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
    
    // Обработка специальных клавиш (Set 2)
    if (scancode == 0x12 || scancode == 0x59) { // Left/Right Shift pressed
        isShiftPressed = true;
        return 0;
    }
    if (scancode == 0xF0) { // Release code prefix
        // Просто игнорируем release коды
        return 0;
    }
    
    // Игнорируем release коды (если пришли без префикса)
    if (scancode & 0x80) {
        return 0;
    }
    
    char ascii = scancodeToAscii(scancode);
    
    // Применяем Shift и Caps Lock (упрощённо)
    if (ascii >= 'a' && ascii <= 'z') {
        if (isShiftPressed || isCapsLock) {
            ascii = ascii - 'a' + 'A';
        }
    }
    
    return ascii;
}

char Keyboard::scancodeToAscii(uint8_t scancode) {
    // Простая таблица для Set 2 (основные клавиши)
    switch (scancode) {
        case 0x16: return '1'; case 0x1E: return '2'; case 0x26: return '3';
        case 0x25: return '4'; case 0x2E: return '5'; case 0x36: return '6';
        case 0x3D: return '7'; case 0x3E: return '8'; case 0x46: return '9';
        case 0x45: return '0';
        
        case 0x15: return 'q'; case 0x1D: return 'w'; case 0x24: return 'e';
        case 0x2D: return 'r'; case 0x2C: return 't'; case 0x35: return 'y';
        case 0x3C: return 'u'; case 0x43: return 'i'; case 0x44: return 'o';
        case 0x4D: return 'p';
        
        case 0x1C: return 'a'; case 0x1B: return 's'; case 0x23: return 'd';
        case 0x2B: return 'f'; case 0x34: return 'g'; case 0x33: return 'h';
        case 0x3B: return 'j'; case 0x42: return 'k'; case 0x4B: return 'l';
        
        case 0x2A: return 'z'; case 0x22: return 'x'; case 0x21: return 'c';
        case 0x2F: return 'v'; case 0x32: return 'b'; case 0x31: return 'n';
        case 0x3A: return 'm';
        
        case 0x29: return ' ';
        case 0x5A: return '\n';
        case 0x66: return '\b';
        case 0x0E: return '`';
        case 0x4E: return '-';
        case 0x55: return '=';
        case 0x54: return '[';
        case 0x5B: return ']';
        case 0x5D: return '\\';
        case 0x4C: return ';';
        case 0x52: return '\'';
        case 0x41: return ',';
        case 0x49: return '.';
        case 0x4A: return '/';
        
        default: return 0;
    }
}