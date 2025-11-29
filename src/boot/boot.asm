ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start: 
    jmp short step2
    nop
times 33 db 0

step2: 
    jmp 0:start

start:
    cli ; Clears interupts
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables interupts

    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; jmp CODE_SEG:load32
    jmp $

gdt_start:
gdt_null:
    dd 0
    dd 0
gdt_code:
    dw 0xffff
    dw 0
    db 0
    db 0x9a
    db 11001111b
    db 0
gdt_data:
    dw 0xffff
    dw 0
    db 0
    db 0x92
    db 11001111b
    db 0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start -1
    dd gdt_start



times 510-($ - $$) db 0
dw 0xAA55


