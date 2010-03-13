#include "sys.h"
#include "video/vgabase.h"
#include "libs/memory.h"

void vga_writeregs(unsigned char *regs)
{
//Varaibles
	unsigned int i;

//Write Miscellaneous register
	outb(0x3C2,*regs);
	regs++;

//Write sequencer registers
	for(i=0;i<5;i++)
	{
		outb(0x3C4,i);
		outb(0x3C5,*regs);
		regs++;
	}

//Unlock CTRC registers
	outb(0x3D4,0x03);
	outb(0x3D5,inb(0x3D5)|0x80);
	outb(0x3D4,0x11);
	outb(0x3D5,inb(0x3D5)&~0x80);

//Make sure they stay unlocked
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

//Write CTRC registers
	for(i=0;i<25;i++)
	{
		outb(0x3D4,i);
		outb(0x3D5,*regs);
		regs++;
	}

//Write Graphics Controller registers
	for(i=0;i<9;i++)
	{
		outb(0x3CE,i);
		outb(0x3CF,*regs);
		regs++;
	}

//Write Attribute Controller registers
	for(i=0;i<21;i++)
	{
		(void)inb(0x3DA);
		outb(0x3C0,i);
		outb(0x3C0,*regs);
		regs++;
	}

//Lock 16-color palette and unblank display
	(void)inb(0x3DA);
	outb(0x3C0,0x20);
}

void vga_setplane(unsigned int p)
{
//Variables
	unsigned char pmask;

//Calculate mask
	p&=3;
	pmask=1<<p;

//Set read plane
	outb(0x3CE,4);
	outb(0x3CF,p);

//Set write plane
	outb(0x3C4,2);
	outb(0x3C5,pmask);
}

unsigned vga_getframebuffersegment()
{
//Variables
	unsigned seg;

//Get seg from GC
	outb(0x3CE,6);
	seg=inb(0x3CF);

//Workout segment number
	seg>>=2;
	seg&=3;
	switch(seg)
	{
	case 0:
	case 1:
		seg=0xA000;
		break;
	case 2:
		seg=0xB000;
		break;
	case 3:
		seg=0xB800;
		break;
	}
	return seg;
}


/*****************************************************************************
SET TEXT MODES
*****************************************************************************/
/*
void set_text_mode(int hi_res)
{
	unsigned rows, cols, ht, i;

	if(hi_res)
	{
		vga_writeregs(g_90x60_text);
		cols = 90;
		rows = 60;
		ht = 8;
	}
	else
	{
		vga_writeregs(g_80x25_text);
		cols = 80;
		rows = 25;
		ht = 16;
	}

// tell the BIOS what we've done, so BIOS text output works OK
	pokew(0x40,0x4A,cols);	// columns on screen
	pokew(0x40,0x4C,cols*rows*2); // framebuffer size
	pokew(0x40,0x50,0);		// cursor pos'n
	pokeb(0x40,0x60,ht-1);	//cursor shape
	pokeb(0x40,0x61,ht-2);
	pokeb(0x40,0x84,rows-1);	// rows on screen - 1
	pokeb(0x40,0x85,ht);		// char height
}
*/
/*****************************************************************************
DEMO GRAPHICS MODES
*****************************************************************************/
/*
static void demo_graphics(void)
{
// 4-color
	vga_writeregs(g_320x200x4);
	g_vgabase_wd = 320;
	g_vgabase_ht = 200;
	g_write_pixel = write_pixel2;
	draw_x();
// 16-color
	vga_writeregs(g_640x480x16);
	g_vgabase_wd = 640;
	g_vgabase_ht = 480;
	g_write_pixel = write_pixel4p;
	draw_x();
// 256-color
	vga_writeregs(g_320x200x256);
	g_vgabase_wd = 320;
	g_vgabase_ht = 200;
	g_write_pixel = write_pixel8;
	draw_x();
// 256-color Mode-X
	vga_writeregs(g_320x200x256_modex);
	g_vgabase_wd = 320;
	g_vgabase_ht = 200;
	g_write_pixel = write_pixel8x;
	draw_x();
// go back to 80x25 text mode
	set_text_mode(0);
}
*/
