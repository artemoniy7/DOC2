; src/boot/boot.asm
[org 0x7C00]
[bits 16]

start:
    mov [boot_drive], dl
    mov ax, 0x0003
    int 0x10
    
    mov si, msg
    call print
    
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x0000
    mov ah, 0x02
    mov al, 50
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc error
    
    mov si, msg_ok
    call print
    
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:start_pm

[bits 32]
start_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000
    
    mov edi, 0xB8000
    mov esi, msg_pm
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0F
    stosw
    jmp .loop
.done:
    
    call 0x10000
    cli
    hlt

[bits 16]
print:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

error:
    mov si, msg_err
    call print
    cli
    hlt

msg: db "Loading kernel...", 0x0D, 0x0A, 0
msg_ok: db "Kernel loaded, switching to protected mode...", 0x0D, 0x0A, 0
msg_err: db "Disk read error!", 0
msg_pm: db "Protected mode active!", 0
boot_drive: db 0

gdt_start:
    dd 0, 0
gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 510 - ($ - $$) db 0
dw 0xAA55