[BITS 32]

extern real_to_prot
extern prot_to_real

global vbe_get_controller_info
global vbe_get_mode_info
global vbe_set_mode
global vbe_get_mode
global vbe_save_restore_state
global vbe_display_window_control
global vbe_scan_code_length
global vbe_display_start
global vbe_dac_palette
global vbe_palette_data

vbe_store_eax: dd 0
vbe_store_addr: dd 0
vbe_store_edi: dd 0
vbe_store_ebx: dd 0

vbe_fix_ptr:
    push eax
    mov ebx, [vbe_store_addr]
    add ebx, [esp+8]
    mov eax, [ebx]
    mov ecx, eax
    shr eax, 16
    shl eax, 4
    and ecx, 0xFFFF
    add eax, ecx
    mov [ebx], eax
    pop eax
    ret 4

vbe_call_bios:
    ; Save AX
    mov [vbe_store_eax], eax

    call prot_to_real
    [BITS 16]

    ; Set ES
    mov eax, [vbe_store_addr]
    shr eax, 4
    and ax, 0xF000
    mov es, ax

    ; Restore AX
    mov eax, [vbe_store_eax]

    ; BIOS interrupt
    int 0x10

    ; Save AX
    mov [vbe_store_eax], eax

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

    ; Convert real mode far pointers to 32-bit flat pointers
    push 6
    call vbe_fix_ptr
    push 14
    call vbe_fix_ptr
    push 22
    call vbe_fix_ptr
    push 26
    call vbe_fix_ptr
    push 30
    call vbe_fix_ptr

    ; Return
    mov edi, [vbe_store_edi]
    ret

vbe_get_mode_info:
    mov [vbe_store_edi], edi
    mov [vbe_store_ebx], ebx

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
    mov ebx, [vbe_store_ebx]
    mov edi, [vbe_store_edi]
    ret

vbe_set_mode:
    mov [vbe_store_edi], edi
    mov [vbe_store_ebx], ebx

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
    mov ebx, [vbe_store_ebx]
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

vbe_save_restore_state:
    mov [vbe_store_ebx], ebx
    xor ebx, ebx

    ; Function 04
    mov eax, 0x4F04
    ; Parameter 1 -> DL
    mov edx, [esp+4]
    ; Parameter 2 -> CX
    mov ecx, [esp+8]
    ; Parameter 3 -> ES:BX
    mov ebx, [esp+12]
    mov [vbe_store_addr], ebx
    ; Call bios
    call vbe_call_bios
    ; If buffer size is being returned
    mov edx, [esp+4]
    cmp edx, 0
    jne vbe_save_restore_state_return
    ; BX -> Parameter 3
    mov ecx, [esp+12]
    mov [ecx], ebx
    ; Return
vbe_save_restore_state_return:
    mov ebx, [vbe_store_ebx]
    ret

vbe_display_window_control:
    mov [vbe_store_ebx], ebx
    xor edx, edx

    ; Function 05
    mov eax, 0x4F05
    ; Parameter 1 -> BX
    mov ebx, [esp+4]
    ; [Parameter 2] -> DX
    mov edx, [esp+8]
    mov edx, [edx]
    ; Call bios
    call vbe_call_bios
    ; BX -> Parameter 1
    mov ebx, [esp+8]
    mov [ebx], edx
    ; Return
    mov ebx, [vbe_store_ebx]
    ret

vbe_scan_code_length:
    mov [vbe_store_ebx], ebx
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx

    ; Function 06
    mov eax, 0x4F06
    ; Parameter 1 -> BL
    mov ebx, [esp+4]

    cmp ebx, 0
    jne vbe_scan_code_length_l1
    ; [Parameter 3] -> CX
    mov ecx, [esp+12]
    mov ecx, [ecx]
vbe_scan_code_length_l1:
    cmp ebx, 2
    jne vbe_scan_code_length_l2
    ; [Parameter 2] -> CX
    mov ecx, [esp+8]
    mov ecx, [ecx]
vbe_scan_code_length_l2:
    ; Call bios
    call vbe_call_bios
    ; BX -> Parameter 2
    push eax
    mov eax, [esp+12]
    cmp eax, 0
    je vbe_scan_code_length_l3
    mov [eax], ebx
vbe_scan_code_length_l3:
    mov eax, [esp+16]
    cmp eax, 0
    je vbe_scan_code_length_l4
    mov [eax], ecx
vbe_scan_code_length_l4:
    mov eax, [esp+20]
    cmp eax, 0
    je vbe_scan_code_length_l5
    mov [eax], edx
vbe_scan_code_length_l5:
    pop eax
    ; Return
    mov ebx, [vbe_store_ebx]
    ret

vbe_display_start:
    mov [vbe_store_ebx], ebx
    xor ecx, ecx
    xor edx, edx

    ; Function 07
    mov eax, 0x4F07
    ; Parameter 1 -> BX
    mov ebx, [esp+4]
    ; Parameter 2 -> ECX
    mov ecx, [esp+8]
    ; Parameter 3 -> EDX
    mov edx, [esp+12]
    ; Call bios
    call vbe_call_bios
    ; CX -> Parameter 4
    mov ebx, [esp+16]
    cmp ebx, 0
    je vbe_display_start_l1
    mov [ebx], ecx
vbe_display_start_l1:
    ; DX -> Parameter 5
    mov ebx, [esp+20]
    cmp ebx, 0
    je vbe_display_start_l2
    mov [ebx], edx
vbe_display_start_l2:
    ; Return
    mov ebx, [vbe_store_ebx]
    ret

vbe_dac_palette:
    mov [vbe_store_ebx], ebx

    ; Function 08
    mov eax, 0x4F08
    ; Parameter 1 -> BL
    mov bl, [esp+4]
    ; Parameter 2 -> BH
    mov ecx, [esp+8]
    mov bh, [ecx]
    ; Call bios
    call vbe_call_bios
    ; BH -> Parameter 2
    mov ecx, [esp+8]
    mov [ecx], dword 0
    mov [ecx], bh
    ; Return
    mov ebx, [vbe_store_ebx]
    ret

vbe_palette_data:
    mov [vbe_store_ebx], ebx
    mov [vbe_store_edi], edi

    ; Function 09
    mov eax, 0x4F09
    ; Parameter 1 -> BL
    mov ebx, [esp+4]
    ; Parameter 2 -> CX
    mov ebx, [esp+8]
    ; Parameter 3 -> DX
    mov ebx, [esp+12]
    ; Parameter 4 -> ES:DI
    mov edi, [esp+16]
    mov [vbe_store_addr], edi
    ; Call bios
    call vbe_call_bios
    ; Return
    mov edi, [vbe_store_edi]
    mov ebx, [vbe_store_ebx]
    ret
