// src/kernel/start.cpp
#include "kernel/kernel.h"

extern "C" void _start() {
    // Вызываем главную функцию ядра
    kernel_main();
}