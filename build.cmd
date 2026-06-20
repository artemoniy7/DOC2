@echo off
echo ========================================
echo Building OC - Protected Mode Kernel
echo ========================================

if not exist build mkdir build

:: Шаг 1: Загрузчик
echo [1/6] Assembling bootloader...
nasm -f bin src\boot\boot.asm -o build\boot.bin
if errorlevel 1 (
    echo ERROR: Bootloader assembly failed!
    pause
    exit /b 1
)
echo [+] boot.bin created

:: Шаг 2: Компиляция
echo [2/6] Compiling kernel...
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\kernel.cpp -o build\kernel.o
if errorlevel 1 goto error

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\console.cpp -o build\console.o
if errorlevel 1 goto error

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\keyboard.cpp -o build\keyboard.o
if errorlevel 1 goto error

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\shell.cpp -o build\shell.o
if errorlevel 1 goto error
echo [+] Object files created

:: Шаг 3: Линковка в PE
echo [3/6] Linking kernel...
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -nostdlib -Wl,-T,src\linker.ld -Wl,-Ttext,0x10000 -Wl,--subsystem,native -o build\kernel.exe build\kernel.o build\console.o build\keyboard.o build\shell.o
if errorlevel 1 goto error
echo [+] kernel.exe created

:: Шаг 4: Конвертация
echo [4/6] Converting to binary...
objcopy -O binary build\kernel.exe build\kernel.bin
if errorlevel 1 goto error

:: Проверка размера
for %%A in (build\kernel.bin) do set KERNEL_SIZE=%%~zA
if !KERNEL_SIZE! equ 0 (
    echo ERROR: kernel.bin is empty!
    goto error
)
echo [+] kernel.bin created (!KERNEL_SIZE! bytes)

:: Шаг 5: Сборщик образа
echo [5/6] Compiling image builder...
g++ -std=c++17 -O2 -Wall -Wextra -pedantic src\build_image.cpp -o build\build_image.exe
if errorlevel 1 goto error
echo [+] build_image.exe created

:: Шаг 6: Сборка образа
echo [6/6] Building disk image...
build\build_image.exe build\os.img
if errorlevel 1 goto error
echo [+] os.img created

echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo.
dir build\*.bin build\*.img
echo.

choice /C YN /M "Run QEMU now?"
if errorlevel 2 goto end
qemu-system-i386 -drive format=raw,file=build\os.img -no-reboot -no-shutdown

:end
pause
exit /b 0

:error
echo.
echo ========================================
echo BUILD FAILED!
echo ========================================
pause
exit /b 1