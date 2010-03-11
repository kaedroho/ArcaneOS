;This file gets all the values from CPUID and returns them in a structure

[BITS 32]

extern cpuid_main

global cpuid_run
cpuid_run:
    mov eax,[cpuid_main]
    cpuid
    mov [cpuid_main],eax
    mov [cpuid_main+4],ebx
    mov [cpuid_main+8],ecx
    mov [cpuid_main+12],edx
    ret
