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

    jmp CODE_SEG:load32

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

[BITS 32]
load32:
    mov eax, 1
    mov ecx, 100
    mov edi, 0x0100000
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax ; Backup lba
    shr eax, 24 ; set hughest 8 bits. Shift right 24
    or eax, 0xE0 ; select master drive
    mov dx, 0x1F6
    out dx, al
    ; Finished sending  the highest 8 bits to lba
    
    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al

    ; Finish Send the total sectors to read
    mov eax, ebx ; Restore backup
    mov dx, 0x1F3
    out dx, al
    ; Finish sending more bits to lba

    mov dx, 0x1F4
    mov eax, ebx
    shr eax, 8
    out dx, al
    ; Send more bits
    
    ; Send upper bits
    mov dx, 0x1F5
    mov eax, ebx
    shr eax, 16
    out dx, al
    ;; FInish sending upper bits


    mov dx, 0x1f7
    mov al, 0x20
    out dx, al
    
    ;; Read all sectors into memory

.next_sector:
    push ecx

.try_again:
    mov dx, 0x1F7
    in al, dx
    test al, 8
    jz .try_again

    mov ecx, 256
    mov dx, 0x1f
    rep insw
    pop ecx
    loop .next_sector
    ;; End of reading sectores
    ret




times 510-($ - $$) db 0
dw 0xAA55


