/* 
 * File:   gdt.h
 * Author: Diggory
 *
 * Created on 05 November 2010, 00:06
 */

#ifndef GDT_H
#define	GDT_H

#define GDT_NULL_SEG                0x00
#define GDT_KERNEL_PROT_MODE_CSEG   0x08
#define GDT_KERNEL_PROT_MODE_DSEG   0x10
#define GDT_KERNEL_REAL_MODE_CSEG   0x18
#define GDT_KERNEL_REAL_MODE_DSEG   0x20
#define GDT_TASK_STATE_SEGMENT      0x28
#define GDT_USERMODE_PROT_MODE_CSEG 0x30
#define GDT_USERMODE_PROT_MODE_DSEG 0x38

#define GDT_ACCESS_PRESENT  ((1<<7)|(1<<4))
#define GDT_ACCESS_RING(n)  (n<<5)
#define GDT_ACCESS_KERNEL   GDT_ACCESS_RING(0)
#define GDT_ACCESS_USERMODE GDT_ACCESS_RING(3)
#define GDT_ACCESS_DATA  (0<<3)
#define GDT_ACCESS_CODE  (1<<3)
#define GDT_ACCESS_DATA_GROW_UP  (1<<2)
#define GDT_ACCESS_CODE_CONFORMING  (1<<2)
#define GDT_ACCESS_DATA_WRITABLE  (1<<1)
#define GDT_ACCESS_CODE_READABLE  (1<<1)
#define GDT_ACCESS_ACCESSED (1<<0)

#define GDT_GRAN_BYTES  (0<<7)
#define GDT_GRAN_PAGES  (1<<7)
#define GDT_GRAN_16BIT  (0<<6)
#define GDT_GRAN_32BIT  (1<<6)

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

struct gdt_tss {
    unsigned short link;
    unsigned short reserved0;
    unsigned int esp0;
    unsigned short ss0;
    unsigned short reserved1;
    unsigned int esp1;
    unsigned short ss1;
    unsigned short reserved2;
    unsigned int esp2;
    unsigned short ss2;
    unsigned short reserved3;
    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned short es;
    unsigned short reserved4;
    unsigned short cs;
    unsigned short reserved5;
    unsigned short ss;
    unsigned short reserved6;
    unsigned short ds;
    unsigned short reserved7;
    unsigned short fs;
    unsigned short reserved8;
    unsigned short gs;
    unsigned short reserved9;
    unsigned short ldtr;
    unsigned short reserved10;
    unsigned short reserved11;
    unsigned short iobp;
} __attribute__((packed));

#define GDT_COUNT 8

//Allocate 4 GDT entries. NULL, CS, DS, TSS.
extern struct gdt_entry g_gdt[GDT_COUNT];

//GDT Pointer
extern struct gdt_ptr g_gdtp;

// TSS Pointer
extern struct gdt_tss g_tss;

#endif	/* GDT_H */

