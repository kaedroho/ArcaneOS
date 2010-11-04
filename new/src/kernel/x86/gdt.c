#include <sys-x86.h>

//GDT entry structure
struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

//GDT pointer structure
struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

#define GDT_COUNT 8

//Allocate 4 GDT entries. NULL, CS, DS, TSS.
struct gdt_entry g_gdt[GDT_COUNT];

//GDT Pointer
struct gdt_ptr g_gdtp;

//Import functions from gdt.asm
extern void gdt_setup();


void gdt_set_gate(int num,unsigned long base,unsigned long limit,unsigned char access,unsigned char gran)
{
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
    gdt_set_gate(0,0,0,0,0);

//Code segment
    gdt_set_gate(1,0,0xFFFFFFFF,0x9A,0xCF);

//Data segment
    gdt_set_gate(2,0,0xFFFFFFFF,0x92,0xCF);

//Setup GDT
    gdt_setup();
}