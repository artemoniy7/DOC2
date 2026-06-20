@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo Building OC - Protected Mode Kernel
echo ========================================
echo.

:: Создание папки build
if not exist build mkdir build

:: Шаг 1: Сборка загрузчика
echo [1/6] Assembling bootloader...
nasm -f bin src\boot\boot.asm -o build\boot.bin 2>nul
if errorlevel 1 (
    echo ERROR: Bootloader assembly failed!
    echo Trying alternative bootloader...
    
    :: Используем упрощённый загрузчик
    (
        echo [org 0x7C00]
        echo [bits 16]
        echo.
        echo start:
        echo     mov [boot_drive], dl
        echo     mov ax, 0x0003
        echo     int 0x10
        echo     mov si, msg
        echo     call print
        echo.
        echo     mov ax, 0x1000
        echo     mov es, ax
        echo     mov bx, 0x0000
        echo     mov ah, 0x02
        echo     mov al, 50
        echo     mov ch, 0
        echo     mov cl, 2
        echo     mov dh, 0
        echo     mov dl, [boot_drive]
        echo     int 0x13
        echo     jc error
        echo.
        echo     mov si, msg_ok
        echo     call print
        echo.
        echo     cli
        echo     lgdt [gdt_descriptor]
        echo     mov eax, cr0
        echo     or eax, 1
        echo     mov cr0, eax
        echo     jmp CODE_SEG:start_pm
        echo.
        echo [bits 32]
        echo start_pm:
        echo     mov ax, DATA_SEG
        echo     mov ds, ax
        echo     mov es, ax
        echo     mov ss, ax
        echo     mov esp, 0x90000
        echo     mov edi, 0xB8000
        echo     mov esi, msg_pm
        echo .loop:
        echo     lodsb
        echo     or al, al
        echo     jz .done
        echo     mov ah, 0x0F
        echo     stosw
        echo     jmp .loop
        echo .done:
        echo     call 0x10000
        echo     cli
        echo     hlt
        echo.
        echo [bits 16]
        echo print:
        echo     lodsb
        echo     or al, al
        echo     jz .done
        echo     mov ah, 0x0E
        echo     int 0x10
        echo     jmp print
        echo .done:
        echo     ret
        echo.
        echo error:
        echo     mov si, msg_err
        echo     call print
        echo     cli
        echo     hlt
        echo.
        echo msg: db "Loading...", 0x0D, 0x0A, 0
        echo msg_ok: db "OK!", 0x0D, 0x0A, 0
        echo msg_err: db "ERROR!", 0
        echo msg_pm: db "Protected mode OK", 0
        echo boot_drive: db 0
        echo.
        echo gdt_start:
        echo     dd 0, 0
        echo gdt_code:
        echo     dw 0xFFFF
        echo     dw 0x0000
        echo     db 0x00
        echo     db 10011010b
        echo     db 11001111b
        echo     db 0x00
        echo gdt_data:
        echo     dw 0xFFFF
        echo     dw 0x0000
        echo     db 0x00
        echo     db 10010010b
        echo     db 11001111b
        echo     db 0x00
        echo gdt_end:
        echo.
        echo gdt_descriptor:
        echo     dw gdt_end - gdt_start - 1
        echo     dd gdt_start
        echo.
        echo CODE_SEG equ gdt_code - gdt_start
        echo DATA_SEG equ gdt_data - gdt_start
        echo.
        echo times 510 - ($ - $$) db 0
        echo dw 0xAA55
    ) > build\boot_temp.asm
    
    nasm -f bin build\boot_temp.asm -o build\boot.bin 2>nul
    if errorlevel 1 (
        echo ERROR: Alternative bootloader also failed!
        pause
        exit /b 1
    )
    echo [+] boot.bin created (using alternative)
) else (
    echo [+] boot.bin created
)

:: Шаг 2: Компиляция ядра
echo [2/6] Compiling kernel...

:: Проверка наличия исходников
if not exist src\kernel\kernel.cpp (
    echo ERROR: src\kernel\kernel.cpp not found!
    pause
    exit /b 1
)

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\kernel.cpp -o build\kernel.o
if errorlevel 1 goto error

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\console.cpp -o build\console.o
if errorlevel 1 goto error

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\keyboard.cpp -o build\keyboard.o
if errorlevel 1 goto error

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -I src -c src\kernel\shell.cpp -o build\shell.o
if errorlevel 1 goto error

echo [+] Object files created

:: Шаг 3: Линковка ядра
echo [3/6] Linking kernel...

:: Пробуем разные способы линковки
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -nostdlib -Wl,-T,src\linker.ld -Wl,-Ttext,0x10000 -Wl,--oformat,elf32-i386 -o build\kernel.elf build\kernel.o build\console.o build\keyboard.o build\shell.o 2>nul

if errorlevel 1 (
    echo Trying alternative linking...
    g++ -std=c++17 -O2 -Wall -Wextra -pedantic -ffreestanding -fno-exceptions -fno-rtti -m32 -nostdlib -Wl,-T,src\linker.ld -Wl,-Ttext,0x10000 -o build\kernel.elf build\kernel.o build\console.o build\keyboard.o build\shell.o 2>nul
)

if errorlevel 1 (
    echo ERROR: Linking failed!
    goto error
)
echo [+] kernel.elf created

:: Шаг 4: Конвертация в бинарник
echo [4/6] Converting to binary...

objcopy -O binary build\kernel.elf build\kernel.bin 2>nul
if errorlevel 1 (
    echo ERROR: objcopy failed!
    goto error
)

:: Проверка размера
for %%A in (build\kernel.bin) do set KERNEL_SIZE=%%~zA
if !KERNEL_SIZE! equ 0 (
    echo ERROR: kernel.bin is empty!
    goto error
)

echo [+] kernel.bin created (!KERNEL_SIZE! bytes)

:: Шаг 5: Компиляция сборщика образа
echo [5/6] Compiling image builder...

g++ -std=c++17 -O2 -Wall -Wextra -pedantic src\build_image.cpp -o build\build_image.exe
if errorlevel 1 goto error
echo [+] build_image.exe created

:: Шаг 6: Сборка образа
echo [6/6] Building disk image...

build\build_image.exe build\os.img
if errorlevel 1 goto error
echo [+] os.img created

:: Готово!
echo.
echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo.
echo Files in build:
dir build\*.bin build\*.img
echo.

:: Запуск QEMU
choice /C YN /M "Run QEMU now?"
if errorlevel 2 goto end

echo.
echo Starting QEMU...
echo Press Ctrl+Alt+2 to enter QEMU monitor, then type 'quit' to exit
echo.
qemu-system-i386 -drive format=raw,file=build\os.img -no-reboot -no-shutdown

:end
echo.
pause
exit /b 0

:error
echo.
echo ========================================
echo BUILD FAILED!
echo ========================================
echo.
echo Check the error messages above.
echo.
pause
exit /b 1