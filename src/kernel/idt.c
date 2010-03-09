#include "sys.h"
#include "libs/memory.h"

//IDT entry structure
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

//IDT pointer structure
struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));


//Allocate 256 IDT entries
struct idt_entry idt[256];

//IDT Pointer
struct idt_ptr idtp;

//Import functions from idt.asm
extern void idt_load();


void idt_set_gate(unsigned char num,unsigned long base,unsigned short sel,unsigned char flags)
{
//Add bases to IDT
    idt[num].base_lo=base & 0xFFFF;
    idt[num].base_hi=(base>>16) & 0xFFFF;

//Set the other values
    idt[num].sel=sel;
    idt[num].always0=0;
    idt[num].flags=flags;
}

void idt_init()
{
//Setup GDT pointer
    idtp.limit=(sizeof(struct idt_entry)*256)-1;
    idtp.base=(unsigned int)&idt;

//Zero IDT
    memset((unsigned char*)&idt,0,sizeof(struct idt_entry)*256);

//Load IDT
    idt_load();
}
