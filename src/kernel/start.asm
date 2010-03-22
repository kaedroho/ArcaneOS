;Kernel entry point!

[BITS 32]
global start
start:
    ;stack pointer
    mov esp, _sys_stack
    jmp stublet

;This part has to be 4 byte aligned
ALIGN 4
mboot:
    ; Multiboot macros to make a few lines later more readable
    MULTIBOOT_PAGE_ALIGN	equ 1<<0
    MULTIBOOT_MEMORY_INFO	equ 1<<1
    MULTIBOOT_AOUT_KLUDGE	equ 1<<16
    MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    EXTERN code, bss, end

    ; This is the GRUB Multiboot header. A boot signature
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM

    ; AOUT kludge - must be physical addresses. Make a note of these:
    ; The linker script fills in the data for these ones!
    dd mboot
    dd code
    dd bss
    dd end
    dd start


;Call kernel main funciton
stublet:
    ; Get multiboot info from bootloader
    extern g_multiboot_info
    mov [g_multiboot_info], ebx

    extern main
    call main
    jmp $


SECTION .data

global real_start
global real_end
global _paging_stack

real_start:
    incbin "bin/real.bin"
real_end:

;Store stack in BSS section
SECTION .bss
    resb 8192 ;Reserve 8 KB of memory
_sys_stack:
    resb 4096 ;Reserve 4 KB of memory
_paging_stack:
