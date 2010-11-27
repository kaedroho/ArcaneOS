#include <string.h>

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
struct idt_entry idt_idt[256];

//IDT Pointer
struct idt_ptr idt_idtp;

//Import functions from idt.asm
extern void idt_load();


void idt_set_gate(unsigned char num,unsigned long base,unsigned short sel,unsigned char flags)
{
//Add bases to IDT
    idt_idt[num].base_lo=base & 0xFFFF;
    idt_idt[num].base_hi=(base>>16) & 0xFFFF;

//Set the other values
    idt_idt[num].sel=sel;
    idt_idt[num].always0=0;
    idt_idt[num].flags=flags;
}

void idt_init()
{
//Setup GDT pointer
    idt_idtp.limit=(sizeof(struct idt_entry)*256)-1;
    idt_idtp.base=(unsigned int)&idt_idt;

//Zero IDT
    memset((unsigned char*)&idt_idt,0,sizeof(struct idt_entry)*256);

//Load IDT
    idt_load();
}