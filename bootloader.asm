[BITS 16]
[ORG 0x7C00]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    mov [boot_drive], dl

    ; reset do drive
    xor ah, ah
    mov dl, [boot_drive]
    int 0x13

    ; exibir mensagem
    mov ax, 0xB800
    mov es, ax
    mov di, 0
    mov si, msg
print_loop:
    lodsb
    or al, al
    jz load_kernel
    mov ah, 0x0A
    mov [es:di], ax
    add di, 2
    jmp print_loop

load_kernel:
    mov ax, 0x07E0     ; segmento 0x07E0 * 16 = 0x7E00
    mov es, ax
    xor bx, bx         ; offset 0
    mov ah, 0x02
    mov al, 32
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:kernel_entry

disk_error:
    mov ax, 0xB800
    mov es, ax
    mov di, 160
    mov si, err_msg
err_loop:
    lodsb
    or al, al
    jz halt
    mov ah, 0x0C
    mov [es:di], ax
    add di, 2
    jmp err_loop

halt:
    cli
    hlt

msg         db "Welcome to shellOS", 0
err_msg     db "SHELL PANIC! disk failed", 0
boot_drive  db 0

gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
kernel_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    call 0x7E00
    cli
    hlt

times 510 - ($ - $$) db 0
dw 0xAA55
