// src/kernel/kernel.cpp
#include "console.h"
#include "keyboard.h"
#include "shell.h"

// Точка входа для линковщика
extern "C" void _start() {
    // Инициализация консоли
    Console console;
    console.clear();
    console.write("Kernel loaded at 0x10000\r\n");
    console.write("Initializing keyboard...\r\n");
    
    Keyboard::init();
    console.write("Keyboard initialized\r\n");
    
    console.write("Starting shell...\r\n");
    Shell shell;
    shell.run();
    
    // Если shell завершился - зависаем
    console.write("\r\nKernel halted\r\n");
    while (1) {
        asm volatile ("hlt");
    }
}