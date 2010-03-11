#include "sys.h"
#include "libs/memory.h"

struct cpuid_struct cpuid_info;

//Functions from cpuid.asm
extern void cpuid_run();

void cpuid_init()
{
//Clear struct
    memset((unsigned char*)&cpuid_info,0,sizeof(struct cpuid_struct));

//Run cpuid
    cpuid_run();
}

struct cpuid_struct* cpuid_getstruct()
{
    return &cpuid_info;
}
