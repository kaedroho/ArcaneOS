[BITS 32]

global pg_enable_paging
global pg_disable_paging
global pg_is_paging_enabled

enabled:
    dd 0

pg_enable_paging:
    mov eax, cr0
    or eax, dword 0x80000000
    mov cr0, eax
    mov [enabled], dword 1
    ret

pg_disable_paging:
    mov eax, cr0
    and eax, dword 0x7FFFFFFF
    mov cr0, eax
    mov [enabled], dword 0
    ret

pg_is_paging_enabled:
    mov eax, [enabled]
    ret