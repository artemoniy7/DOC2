@echo off
set CROSS=C:\cross\bin
set CC=%CROSS%\i686-elf-g++.exe
set AS=%CROSS%\i686-elf-as.exe
set OBJCOPY=%CROSS%\i686-elf-objcopy.exe

echo Building OC kernel with cross-compiler...

if not exist build mkdir build

REM Компилируем загрузчик
nasm -f bin src/boot/boot.asm -o build/bootloader.bin
if errorlevel 1 goto error

REM Компилируем C++ файлы
%CC% -std=c++17 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-stack-protector -fno-builtin -I src -c src/kernel/start.cpp -o build/start.o
%CC% -std=c++17 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-stack-protector -fno-builtin -I src -c src/kernel/kernel_main.cpp -o build/kernel_main.o
%CC% -std=c++17 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-stack-protector -fno-builtin -I src -c src/kernel/console.cpp -o build/console.o
%CC% -std=c++17 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-stack-protector -fno-builtin -I src -c src/kernel/keyboard.cpp -o build/keyboard.o
%CC% -std=c++17 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-stack-protector -fno-builtin -I src -c src/kernel/shell.cpp -o build/shell.o

REM Линкуем
%CC% -ffreestanding -nostdlib -T src/linker.ld -lgcc -o build/kernel.elf build/start.o build/kernel_main.o build/console.o build/keyboard.o build/shell.o
if errorlevel 1 goto error

REM Конвертируем в бинарник
%OBJCOPY% -O binary build/kernel.elf build/kernel.bin

REM Создаём образ
g++ -std=c++17 -I src build_image.cpp -o build/build_image.exe
build\build_image.exe

echo.
echo Build successful!
echo.
echo To run: qemu-system-i386 -drive format=raw,file=build/os.img
goto end

:error
echo.
echo Build failed!
pause
:end