;This file gets all the values from CPUID and returns them in a structure

[BITS 32]

extern cpuid_info

global cpuid_run
cpuid_run:
    mov eax,0x00
    cpuid
    mov [cpuid_info],eax
    mov [cpuid_info+4],ebx
    mov [cpuid_info+8],edx
    mov [cpuid_info+12],ecx

    ret
