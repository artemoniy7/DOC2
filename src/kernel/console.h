// src/kernel/console.h
#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdint>

class Console {
public:
    Console();
    void clear();
    void putChar(char c);
    void write(const char* str);
    void write(const char* str, int len);
    void setColor(uint8_t color);
    void setCursor(int row, int col);
    void getCursor(int& row, int& col);
    void scroll();
    
    static const int WIDTH = 80;
    static const int HEIGHT = 25;
    
private:
    uint16_t* videoMemory;
    int cursorRow;
    int cursorCol;
    uint8_t currentColor;
    
    void updateHardwareCursor();
    void putCharAt(char c, int row, int col, uint8_t color);
};

// Цвета VGA
enum VGAColor {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15
};

// Глобальный экземпляр консоли
extern Console console;

#endif