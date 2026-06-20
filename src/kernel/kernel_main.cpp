// src/kernel/kernel_main.cpp
#include "console.h"
#include "keyboard.h"
#include "shell.h"

void kernel_main() {
    Console console;
    console.clear();
    console.write("Kernel loaded at 0x10000\r\n");
    console.write("Initializing keyboard...\r\n");
    
    Keyboard::init();
    console.write("Keyboard initialized\r\n");
    
    console.write("Starting shell...\r\n");
    Shell shell;
    shell.run();
    
    console.write("\r\nKernel halted\r\n");
    while (1) {
        asm volatile ("hlt");
    }
}