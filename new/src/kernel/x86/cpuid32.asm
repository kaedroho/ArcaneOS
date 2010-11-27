;This file gets all the values from CPUID and returns them in a structure

[BITS 32]

extern cpuid_info

global cpuid_run
cpuid_run:
    mov eax,0x00000000
    cpuid
    mov [cpuid_info],eax
    mov [cpuid_info+4],ebx
    mov [cpuid_info+8],edx
    mov [cpuid_info+12],ecx

    mov eax,0x00000001
    cpuid
    mov [cpuid_info+17],eax
    mov [cpuid_info+21],ebx
    mov [cpuid_info+25],ecx
    mov [cpuid_info+29],edx

    mov eax,0x00000004
    mov ecx,0
    cpuid
    mov [cpuid_info+33],eax
    mov [cpuid_info+37],ebx
    mov [cpuid_info+41],ecx
    mov [cpuid_info+45],edx

    mov eax,0x00000004
    mov ecx,1
    cpuid
    mov [cpuid_info+39],eax
    mov [cpuid_info+53],ebx
    mov [cpuid_info+57],ecx
    mov [cpuid_info+61],edx

    mov eax,0x00000005
    cpuid
    mov [cpuid_info+65],eax
    mov [cpuid_info+69],ebx
    mov [cpuid_info+73],ecx
    mov [cpuid_info+77],edx

    mov eax,0x00000006
    cpuid
    mov [cpuid_info+81],eax
    mov [cpuid_info+85],ebx
    mov [cpuid_info+89],ecx

    mov eax,0x00000007
    cpuid
    mov [cpuid_info+93],eax

    mov eax,0x0000000A
    cpuid
    mov [cpuid_info+97],eax
    mov [cpuid_info+101],ebx
    mov [cpuid_info+105],edx

    ret