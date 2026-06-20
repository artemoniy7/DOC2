[org 0x7C00]
bits 16

start:
    ; Очищаем экран
    mov ax, 0x0003
    int 0x10

    ; Выводим сообщение
    mov si, msg
    call print

    ; Бесконечный цикл (пока нет ядра)
    cli
    hlt
    jmp $

print:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

msg db "OC booted!", 0

times 510 - ($ - $$) db 0
dw 0xAA55