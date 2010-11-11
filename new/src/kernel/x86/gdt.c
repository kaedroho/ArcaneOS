#include <x86/sys.h>
#include <x86/gdt.h>

//Allocate 4 GDT entries. NULL, CS, DS, TSS.
struct gdt_entry g_gdt[GDT_COUNT];

//GDT Pointer
struct gdt_ptr g_gdtp;

//Import functions from gdt.asm
extern void gdt_setup();


void gdt_set_gate(int seg,unsigned long base,unsigned long limit,unsigned char access,unsigned char gran)
{
    int num = seg>>3;
    
//Set base address
    g_gdt[num].base_low =(base & 0xFFFF);
    g_gdt[num].base_middle =(base>>16) & 0xFF;
    g_gdt[num].base_high=(base>>24) & 0xFF;

//Set the other values
    g_gdt[num].limit_low=(limit & 0xFFFF);
    g_gdt[num].granularity=((limit>>16) & 0x0F);
    g_gdt[num].granularity|=(gran & 0xF0);
    g_gdt[num].access=access;
}

void gdt_init()
{
//Setup GDT pointer
    g_gdtp.limit=(sizeof(struct gdt_entry)*GDT_COUNT)-1;
    g_gdtp.base=(unsigned int)&g_gdt;

//NULL Descriptor
    gdt_set_gate(GDT_NULL_SEG,0,0,0,0);

//Kernal protected mode code segment
    gdt_set_gate(
        GDT_KERNEL_PROT_MODE_CSEG, 0, 0xFFFFFFFF,
        GDT_ACCESS_PRESENT|GDT_ACCESS_KERNEL|GDT_ACCESS_CODE|GDT_ACCESS_CODE_READABLE,
        GDT_GRAN_PAGES|GDT_GRAN_32BIT
        );

//Kernal protected mode data segment
    gdt_set_gate(
        GDT_KERNEL_PROT_MODE_DSEG, 0, 0xFFFFFFFF,
        GDT_ACCESS_PRESENT|GDT_ACCESS_KERNEL|GDT_ACCESS_DATA|GDT_ACCESS_DATA_WRITABLE,
        GDT_GRAN_PAGES|GDT_GRAN_32BIT
        );

//Kernal real mode code segment
    gdt_set_gate(
        GDT_KERNEL_REAL_MODE_CSEG, 0, 0xFFFFFFFF,
        GDT_ACCESS_PRESENT|GDT_ACCESS_KERNEL|GDT_ACCESS_CODE|GDT_ACCESS_CODE_READABLE,
        GDT_GRAN_BYTES|GDT_GRAN_16BIT
        );

//Kernal real mode data segment
    gdt_set_gate(
        GDT_KERNEL_REAL_MODE_DSEG, 0, 0xFFFFFFFF,
        GDT_ACCESS_PRESENT|GDT_ACCESS_KERNEL|GDT_ACCESS_DATA|GDT_ACCESS_DATA_WRITABLE,
        GDT_GRAN_BYTES|GDT_GRAN_16BIT
        );

//Setup GDT
    gdt_setup();
}