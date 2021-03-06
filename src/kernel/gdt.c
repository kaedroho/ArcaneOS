#include "sys.h"
#include "mt.h"

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
struct gdt_entry gdt[GDT_COUNT];

//GDT Pointer
struct gdt_ptr gdtp;

//Import functions from gdt.asm
extern void gdt_setup();


void gdt_set_gate(int num,unsigned long base,unsigned long limit,unsigned char access,unsigned char gran)
{
//Set base address
    gdt[num].base_low =(base & 0xFFFF);
    gdt[num].base_middle =(base>>16) & 0xFF;
    gdt[num].base_high=(base>>24) & 0xFF;

//Set the other values
    gdt[num].limit_low=(limit & 0xFFFF);
    gdt[num].granularity=((limit>>16) & 0x0F);
    gdt[num].granularity|=(gran & 0xF0);
    gdt[num].access=access;
}

void gdt_init()
{
//Setup GDT pointer
    gdtp.limit=(sizeof(struct gdt_entry)*GDT_COUNT)-1;
    gdtp.base=(unsigned int)&gdt;

//NULL Descriptor
    gdt_set_gate(0,0,0,0,0);

//Code segment
    gdt_set_gate(1,0,0xFFFFFFFF,0x9A,0xCF);

//Data segment
    gdt_set_gate(2,0,0xFFFFFFFF,0x92,0xCF);

//Code segment
    gdt_set_gate(3,0,0xFFFFFFFF,0xFA,0xCF);

//Data segment
    gdt_set_gate(4,0,0xFFFFFFFF,0xF2,0xCF);

//Task state segment
    gdt_set_gate(5,(unsigned int)&mt_task_state_segment,(unsigned int)&mt_task_state_segment + sizeof(mt_task_state_segment) + 1,0x89,0x40);

//16-bit Code segment
    gdt_set_gate(6,0,0xFFFF,0x9A,0x00);

//16-bit Data segment
    gdt_set_gate(7,0,0xFFFF,0x92,0x00);

//Setup GDT
    gdt_setup();
}
