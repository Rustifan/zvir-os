CODE_SEG equ 0x0b
DATA_SEG equ 0x10
global _start

[BITS 32]
_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp
    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    jmp $

times 512-($ - $$) db 0
