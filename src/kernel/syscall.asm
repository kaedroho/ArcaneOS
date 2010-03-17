[BITS 32]

global syscall
global syscall_receiver

extern syscall_handler

extern mt_thread_stack_ptr;
extern mt_syscall_stack

syscall:
    mov eax,[esp+4];
    int 0x80
    ret

syscall_receiver:
    cli
    push byte 0
    push byte 0

;Push stuff
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp

    mov [mt_thread_stack_ptr],esp
    mov esp,[mt_syscall_stack]

    push eax

;Call IRQ handler
    mov eax, syscall_handler
    call eax

;Pop stuff
    pop eax

    mov esp,[mt_thread_stack_ptr]

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
