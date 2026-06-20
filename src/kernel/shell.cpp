// src/kernel/shell.cpp
#include "shell.h"
#include "console.h"
#include "keyboard.h"
#include <cstdint>

// Цвета для демо
static const uint8_t colors[] = {
    VGA_COLOR_WHITE,
    VGA_COLOR_RED,
    VGA_COLOR_GREEN,
    VGA_COLOR_BLUE,
    VGA_COLOR_CYAN,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_MAGENTA
};

Shell::Shell() : echoMode(false), currentColorIndex(0), running(true) {}

void Shell::run() {
    console.clear();
    console.write("OC kernel shell (protected mode)\r\n");
    console.write("Press a key...\r\n");
    console.write("H help  A about  C clear  R reboot\r\n");
    console.write("E echo  B beep   F color  G position demo\r\n");
    console.write("> ");
    
    while (running) {
        char key = Keyboard::getChar();
        
        if (echoMode) {
            if (key == 27) { // ESC
                echoMode = false;
                console.write("\r\nExiting echo mode\r\n> ");
                continue;
            }
            console.putChar(key);
            continue;
        }
        
        // Normalize to uppercase
        if (key >= 'a' && key <= 'z') {
            key = key - 'a' + 'A';
        }
        
        processCommand(key);
    }
}

void Shell::processCommand(char key) {
    switch (key) {
        case 'H': printHelp(); break;
        case 'A': printAbout(); break;
        case 'C': clearScreen(); break;
        case 'R': reboot(); break;
        case 'E': toggleEchoMode(); break;
        case 'B': 
            console.write("\x07"); // BEL character
            break;
        case 'F': nextColor(); break;
        case 'G': printPositionDemo(); break;
        case 'M': printMemoryInfo(); break;
        case 'K': printKeyboardInfo(); break;
        case 'D': printDiskInfo(); break;
        case 'T': printTaskInfo(); break;
        case 'V': printConsoleInfo(); break;
        case 'S': printSyscallInfo(); break;
        case 'N': printNextSteps(); break;
        case 'P': printProcessInfo(); break;
        default:
            if (key >= 0x20 && key <= 0x7E) {
                console.write("Unknown command: ");
                console.putChar(key);
                console.write("\r\n");
            }
            console.write("> ");
            break;
    }
}

void Shell::printHelp() {
    console.write("\r\n--- OC Shell Help ---\r\n");
    console.write("H - Show help\r\n");
    console.write("A - About OC\r\n");
    console.write("C - Clear screen\r\n");
    console.write("R - Reboot\r\n");
    console.write("E - Echo mode (ESC to exit)\r\n");
    console.write("B - Beep\r\n");
    console.write("F - Next foreground color\r\n");
    console.write("G - Position demo\r\n");
    console.write("M - Memory info\r\n");
    console.write("K - Keyboard info\r\n");
    console.write("D - Disk info\r\n");
    console.write("T - Task manager stub\r\n");
    console.write("V - Console info\r\n");
    console.write("S - Syscall plan\r\n");
    console.write("N - Next steps\r\n");
    console.write("P - Process table stub\r\n");
    console.write("> ");
}

void Shell::printAbout() {
    console.write("\r\nOC - Operating System\r\n");
    console.write("Version: 0.2 (Protected mode)\r\n");
    console.write("Build: " __DATE__ " " __TIME__ "\r\n");
    console.write("Mode: 32-bit Protected Mode\r\n");
    console.write("> ");
}

void Shell::clearScreen() {
    console.clear();
    console.write("> ");
}

void Shell::reboot() {
    console.write("Rebooting...\r\n");
    // Тройная перезагрузка через клавиатурный контроллер
    // В защищённом режиме нет BIOS, используем порты
    asm volatile (
        "cli\n"
        "mov $0x64, %al\n"
        "mov $0xFE, %al\n"
        "out %al, $0x64\n"
        "hlt\n"
    );
}

void Shell::toggleEchoMode() {
    echoMode = !echoMode;
    if (echoMode) {
        console.write("\r\nEcho mode ON (press ESC to exit)\r\n");
    } else {
        console.write("\r\nEcho mode OFF\r\n> ");
    }
}

void Shell::printMemoryInfo() {
    console.write("\r\n--- Memory Info ---\r\n");
    console.write("Video memory: 0xB8000\r\n");
    console.write("Kernel start: 0x10000\r\n");
    console.write("Stack: 0x90000\r\n");
    console.write("Memory map not implemented yet\r\n");
    console.write("> ");
}

void Shell::printKeyboardInfo() {
    console.write("\r\n--- Keyboard Info ---\r\n");
    console.write("Using PS/2 keyboard via port 0x60\r\n");
    console.write("Scancode set 1 (IBM PC/XT)\r\n");
    console.write("Shift and Caps Lock supported\r\n");
    console.write("> ");
}

void Shell::printDiskInfo() {
    console.write("\r\n--- Disk Info ---\r\n");
    console.write("Bootloader loaded from sector 1\r\n");
    console.write("Kernel loaded from sector 2+\r\n");
    console.write("Disk I/O not implemented in protected mode\r\n");
    console.write("> ");
}

void Shell::printTaskInfo() {
    console.write("\r\n--- Task Manager (Stub) ---\r\n");
    console.write("No tasks yet\r\n");
    console.write("Plan: Implement cooperative multitasking\r\n");
    console.write("> ");
}

void Shell::printConsoleInfo() {
    console.write("\r\n--- Console Info ---\r\n");
    console.write("Mode: Text 80x25 VGA\r\n");
    console.write("Video memory: 0xB8000\r\n");
    console.write("Current color: ");
    
    // Вывод текущего цвета как число
    char buf[8];
    int color = currentColorIndex % (sizeof(colors) / sizeof(colors[0]));
    int i = 0;
    int num = colors[color];
    if (num == 0) {
        buf[i++] = '0';
    } else {
        char temp[8];
        int j = 0;
        while (num > 0) {
            temp[j++] = '0' + (num % 10);
            num /= 10;
        }
        while (j > 0) {
            buf[i++] = temp[--j];
        }
    }
    buf[i] = '\0';
    console.write(buf);
    console.write("\r\n> ");
}

void Shell::printSyscallInfo() {
    console.write("\r\n--- Syscall Plan ---\r\n");
    console.write("1. Console output (write)\r\n");
    console.write("2. Console input (read)\r\n");
    console.write("3. Memory allocation\r\n");
    console.write("4. Process management\r\n");
    console.write("> ");
}

void Shell::printNextSteps() {
    console.write("\r\n--- Next Steps ---\r\n");
    console.write("1. Implement IDT and IRQ handlers\r\n");
    console.write("2. Add PIT timer (IRQ0)\r\n");
    console.write("3. Implement keyboard interrupt\r\n");
    console.write("4. Set up paging\r\n");
    console.write("5. Add memory manager\r\n");
    console.write("> ");
}

void Shell::printProcessInfo() {
    console.write("\r\n--- Process Table (Stub) ---\r\n");
    console.write("PID  Name    State\r\n");
    console.write("0    kernel  Running\r\n");
    console.write("Plan: Implement process table\r\n");
    console.write("> ");
}

void Shell::nextColor() {
    currentColorIndex++;
    int color = currentColorIndex % (sizeof(colors) / sizeof(colors[0]));
    console.setColor(colors[color]);
    console.write("\r\nColor changed!\r\n> ");
}

void Shell::printPositionDemo() {
    console.write("\r\n--- Position Demo ---\r\n");
    
    // Сохраняем текущую позицию
    int oldRow, oldCol;
    console.getCursor(oldRow, oldCol);
    
    // Выводим текст в разных позициях
    console.setCursor(10, 20);
    console.setColor(VGA_COLOR_RED);
    console.write("Red text at (10,20)");
    
    console.setCursor(12, 20);
    console.setColor(VGA_COLOR_GREEN);
    console.write("Green text at (12,20)");
    
    console.setCursor(14, 20);
    console.setColor(VGA_COLOR_CYAN);
    console.write("Cyan text at (14,20)");
    
    // Возвращаем курсор
    console.setColor(VGA_COLOR_WHITE);
    console.setCursor(oldRow, oldCol);
    console.write("\r\n> ");
}