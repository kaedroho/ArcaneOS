[BITS 32]

extern real_to_prot
extern prot_to_real

global vbe_get_controller_info
global vbe_get_mode_info
global vbe_set_mode
global vbe_get_mode

vbe_store_eax: dd 0
vbe_store_addr: dd 0
vbe_store_edi: dd 0
vbe_store_ebx: dd 0

vbe_call_bios:
    ; Save AX
    mov [vbe_store_eax], eax

    call prot_to_real
    [BITS 16]

    ; Set ES
    mov eax, [dword vbe_store_addr]
    shr eax, 4
    mov es, ax

    ; Restore AX
    mov eax, [dword vbe_store_eax]

    ; BIOS interrupt
    int 0x10

    ; Save AX
    mov [dword vbe_store_eax], eax

    call real_to_prot
    [BITS 32]

    ; Restore AX
    mov eax, [vbe_store_eax]

    ret

vbe_get_controller_info:
    mov [vbe_store_edi], edi

    ; Function 00
    mov eax, 0x4F00
    ; Parameter 1 -> ES:DI
    mov edi, [esp+4]
    mov [vbe_store_addr], edi
    ; Call bios
    call vbe_call_bios

    ; Return
    mov edi, [vbe_store_edi]
    ret

vbe_get_mode_info:
    mov [vbe_store_edi], edi

    ; Function 01
    mov eax, 0x4F01
    ; Parameter 1 -> CX
    mov ecx, [esp+4]
    ; Parameter 2 -> ES:DI
    mov edi, [esp+8]
    mov [vbe_store_addr], edi
    ; Call bios
    call vbe_call_bios
    ; Return
    mov edi, [vbe_store_edi]
    ret

vbe_set_mode:
    mov [vbe_store_edi], edi

    ; Function 02
    mov eax, 0x4F02
    ; Parameter 1 -> BX
    mov ebx, [esp+4]
    ; Parameter 2 -> ES:DI
    mov edi, [esp+8]
    mov [vbe_store_addr], edi
    ; Call bios
    call vbe_call_bios
    ; Return
    mov edi, [vbe_store_edi]
    ret

vbe_get_mode:
    mov [vbe_store_ebx], ebx
    xor ebx, ebx

    ; Function 03
    mov eax, 0x4F03
    ; Call bios
    call vbe_call_bios
    ; BX -> Parameter 1
    mov ecx, [esp+4]
    mov [ecx], ebx
    ; Return
    mov ebx, [vbe_store_ebx]
    ret
