#include <x86/sys.h>
#include <string.h>

struct cpuid_struct g_cpuid_info;

//Functions from cpuid.asm
extern void cpuid_run();

void cpuid_init()
{
//Clear struct
    memset((unsigned char*)&g_cpuid_info,0,sizeof(struct cpuid_struct));

//Run cpuid
    cpuid_run();
}

struct cpuid_struct* cpuid_getstruct()
{
    return &g_cpuid_info;
}
