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

#define GDT_GRAN_BYTES  (0<<7)
#define GDT_GRAN_PAGES  (1<<7)
#define GDT_GRAN_16BIT  (0<<6)
#define GDT_GRAN_32BIT  (1<<6)

#endif	/* GDT_H */

