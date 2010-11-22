[BITS 32]

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

%macro irq 1
    irq%1:
        cli
        push byte 0
        push byte (%1+32)
        jmp irq_common_stub
%endmacro

irq 0
irq 1
irq 2
irq 3
irq 4
irq 5
irq 6
irq 7
irq 8
irq 9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15

extern irq_handler

irq_common_stub:

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

    ; Push ESP twice
    mov eax, esp
    push eax
    mov eax, esp
    push eax

;Call IRQ handler
    mov eax, irq_handler
    call eax

;Pop stuff
    pop eax
    pop eax

; Switch to new stack if required
    mov esp, eax

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

global irq_lock
global irq_unlock
global irq_query

irq_lock:
    pushf
    pop eax
    cli
    ret

irq_unlock:
    mov eax, [esp+4]
    push eax
    popf;
    ret;

irq_query:
    pushf
    pop eax
    shr eax, 9
    and eax, 1
    ret
