[BITS 32]

GDT_NULL_SEG                equ 0x00
GDT_KERNEL_PROT_MODE_CSEG   equ 0x08
GDT_KERNEL_PROT_MODE_DSEG   equ 0x10
GDT_KERNEL_REAL_MODE_CSEG   equ 0x18
GDT_KERNEL_REAL_MODE_DSEG   equ 0x20

;This sets up the segment registers
global gdt_setup
extern g_gdtp
gdt_setup:
    lgdt [g_gdtp]
    mov ax, GDT_KERNEL_PROT_MODE_DSEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp GDT_KERNEL_PROT_MODE_CSEG:gdt_setup2
gdt_setup2:
    ret


