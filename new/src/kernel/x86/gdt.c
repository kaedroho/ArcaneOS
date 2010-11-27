#include <x86/sys.h>
#include <x86/gdt.h>

//Allocate 4 GDT entries. NULL, CS, DS, TSS.
struct gdt_entry gdt_gdt[GDT_COUNT];

//GDT Pointer
struct gdt_ptr gdt_gdtp;

// Task state segment
struct gdt_tss gdt_tss = {0};

//Import functions from gdt.asm
extern void gdt_setup();


void gdt_set_gate(int seg,unsigned long base,unsigned long limit,unsigned char access,unsigned char gran)
{
    int num = seg>>3;
    
//Set base address
    gdt_gdt[num].base_low =(base & 0xFFFF);
    gdt_gdt[num].base_middle =(base>>16) & 0xFF;
    gdt_gdt[num].base_high=(base>>24) & 0xFF;

//Set the other values
    gdt_gdt[num].limit_low=(limit & 0xFFFF);
    gdt_gdt[num].granularity=((limit>>16) & 0x0F);
    gdt_gdt[num].granularity|=(gran & 0xF0);
    gdt_gdt[num].access=access;
}

void gdt_init()
{
//Setup GDT pointer
    gdt_gdtp.limit=(sizeof(struct gdt_entry)*GDT_COUNT)-1;
    gdt_gdtp.base=(unsigned int)&gdt_gdt;

// Setup task state segment
    gdt_tss.ss0 = GDT_KERNEL_PROT_MODE_DSEG;
    gdt_tss.iobp = sizeof(gdt_tss);

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

// Task state segment
    gdt_set_gate(
        GDT_TASK_STATE_SEGMENT, (unsigned)&gdt_tss, sizeof(gdt_tss),
        0x89,
        GDT_GRAN_BYTES|GDT_GRAN_32BIT
        );

//User protected mode code segment
    gdt_set_gate(
        GDT_USERMODE_PROT_MODE_CSEG, 0, 0xFFFFFFFF,
        GDT_ACCESS_PRESENT|GDT_ACCESS_USERMODE|GDT_ACCESS_CODE|GDT_ACCESS_CODE_READABLE,
        GDT_GRAN_PAGES|GDT_GRAN_32BIT
        );

//User protected mode data segment
    gdt_set_gate(
        GDT_USERMODE_PROT_MODE_DSEG, 0, 0xFFFFFFFF,
        GDT_ACCESS_PRESENT|GDT_ACCESS_USERMODE|GDT_ACCESS_DATA|GDT_ACCESS_DATA_WRITABLE,
        GDT_GRAN_PAGES|GDT_GRAN_32BIT
        );

//Setup GDT
    gdt_setup();
}