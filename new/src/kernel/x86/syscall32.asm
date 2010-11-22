[BITS 32]

global syscall_call
global syscall_interrupt
global syscall_stub

extern syscall_handler

syscall_stub:

;Push stuff
    cli
    push byte 0
    push byte 0x80
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

;Call syscall handler
    mov eax, syscall_handler
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

syscall_interrupt:
    mov eax, [esp+4]
    mov ecx, [esp+8]
    mov edx, [esp+12]
    int 0x80
    ret

syscall_call:
    ; Save ebx
    push ebx

    ; Put parameter bytes into eax
    mov eax, [esp+16]
    ; Put parameter pointer into ecx
    mov ecx, [esp+12]
    ; Put stack pointer in ebx
    mov ebx, esp
    ; Find area on stack large enough for parameters
    sub ebx, eax
    sub ebx, 4

l0:
    ; Subtract parameter bytes by 4
    sub eax, 4
    ; Move data from parameter pointer into stack space
    mov edx, [ecx+eax]
    mov [ebx+eax], edx
    ; Repeat if parameter bytes is more than zero
    jg l0

    ; Put function pointer in eax
    mov eax, [esp+8]
    ; Save esp in ebx, and set esp to stack space containing parameter data
    xchg ebx, esp
    ; Call function pointer
    call eax
    ; Restore esp
    mov esp, ebx

    ; Restore ebx
    pop ebx

    ; Return
    ret
