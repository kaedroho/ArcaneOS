#include <sys-x86.h>

void screen_putc(unsigned short x,unsigned short y,unsigned short c)
{
//Put the character
    *((unsigned short*)0xB8000+y*80+x)=c;
}

unsigned short screen_getc(unsigned short x,unsigned short y)
{
//Get the character
    return *((unsigned short*)0xB8000+y*80+x);
}

void screen_positioncursor(unsigned short x,unsigned short y)
{
//Position Cursor
    unsigned short cp=y*80+x;
    outb(0x3D4,14);
    outb(0x3D5,cp>>8);
    outb(0x3D4,15);
    outb(0x3D5,cp);
}

void screen_putpixel(unsigned short x,unsigned short y,unsigned long p)
{

}

unsigned short screen_getwidth()
{
    return 80;
}

unsigned short screen_getheight()
{
    return 25;
}

unsigned long screen_getframebuffer()
{
    return 0xB8000;
}