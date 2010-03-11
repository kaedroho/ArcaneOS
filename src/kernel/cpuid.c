#include "sys.h"
#include "libs/memory.h"

struct cpuid_main
{
    unsigned int eax,ebx,ecx,edx;
} __attribute__((packed));

struct cpuid_struct cpuid_info;
struct cpuid_main cpuid_main;

//Functions from cpuid.asm
extern void cpuid_run();

void cpuid_init()
{
//Clear struct
    memset((unsigned char*)&cpuid_info,0,sizeof(struct cpuid_struct));

//Fill struct
    cpuid_main.eax=0x00000000;
    cpuid_run();
}

struct cpuid_struct* cpuid_getstruct()
{
    return &cpuid_info;
}
