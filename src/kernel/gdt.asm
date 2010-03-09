[BITS 32]

;This sets up the segment registers
global gdt_setup
extern gdtp
gdt_setup:
    lgdt [gdtp]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:gdt_setup2
gdt_setup2:
    ret
