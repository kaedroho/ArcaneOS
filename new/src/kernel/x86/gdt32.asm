[BITS 32]

%include "x86/gdt32.mac"

;This sets up the segment registers
global gdt_setup
extern gdt_gdtp

gdt_setup:
    ; Load GDT
    lgdt [gdt_gdtp]
    mov ax, GDT_KERNEL_PROT_MODE_DSEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to set CS
    jmp GDT_KERNEL_PROT_MODE_CSEG:gdt_setup2
gdt_setup2:

    ; Load TSS
    mov ax, GDT_TASK_STATE_SEGMENT
    ltr ax

    ; Return
    ret


