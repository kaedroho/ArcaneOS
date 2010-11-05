[BITS 32]

extern real_to_prot
extern prot_to_real

global vbe_get_controller_info

vbe_store_eax: dd 0

vbe_call_bios:
    ; Save AX
    mov [vbe_store_eax], eax

    call prot_to_real
    [BITS 16]

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
    ret