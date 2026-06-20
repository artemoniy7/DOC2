// src/kernel/console.cpp
#include "console.h"

// Вспомогательные функции для работы с портами (inline, чтобы были видны везде)
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

Console console;

Console::Console() 
    : videoMemory((uint16_t*)0xB8000), cursorRow(0), cursorCol(0), currentColor(0x0F) {
    clear();
}

void Console::clear() {
    uint16_t blank = 0x20 | (currentColor << 8);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        videoMemory[i] = blank;
    }
    cursorRow = 0;
    cursorCol = 0;
    updateHardwareCursor();
}

void Console::putChar(char c) {
    if (c == '\n') {
        cursorRow++;
        cursorCol = 0;
    } else if (c == '\t') {
        cursorCol = (cursorCol + 4) & ~3;
    } else if (c == '\r') {
        cursorCol = 0;
    } else if (c == '\b') {
        if (cursorCol > 0) {
            cursorCol--;
            putCharAt(' ', cursorRow, cursorCol, currentColor);
        }
    } else {
        putCharAt(c, cursorRow, cursorCol, currentColor);
        cursorCol++;
        if (cursorCol >= WIDTH) {
            cursorCol = 0;
            cursorRow++;
        }
    }
    
    if (cursorRow >= HEIGHT) {
        scroll();
        cursorRow = HEIGHT - 1;
    }
    
    updateHardwareCursor();
}

void Console::write(const char* str) {
    while (*str) {
        putChar(*str++);
    }
}

void Console::write(const char* str, int len) {
    for (int i = 0; i < len; i++) {
        putChar(str[i]);
    }
}

void Console::setColor(uint8_t color) {
    currentColor = color;
}

void Console::setCursor(int row, int col) {
    if (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH) {
        cursorRow = row;
        cursorCol = col;
        updateHardwareCursor();
    }
}

void Console::getCursor(int& row, int& col) {
    row = cursorRow;
    col = cursorCol;
}

void Console::scroll() {
    // Сдвигаем все строки вверх
    for (int row = 1; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            videoMemory[(row - 1) * WIDTH + col] = videoMemory[row * WIDTH + col];
        }
    }
    // Очищаем последнюю строку
    uint16_t blank = 0x20 | (currentColor << 8);
    for (int col = 0; col < WIDTH; col++) {
        videoMemory[(HEIGHT - 1) * WIDTH + col] = blank;
    }
}

void Console::putCharAt(char c, int row, int col, uint8_t color) {
    if (row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH) {
        videoMemory[row * WIDTH + col] = (uint16_t)c | (color << 8);
    }
}

void Console::updateHardwareCursor() {
    uint16_t pos = cursorRow * WIDTH + cursorCol;
    
    // Обновление аппаратного курсора VGA
    // Порт 0x3D4 - индексный регистр, 0x3D5 - данные
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}