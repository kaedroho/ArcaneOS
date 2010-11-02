#include <sys.h>

unsigned char cmos_disablenmi;

void cmos_init()
{
//NMI is disabled
    cmos_disablenmi=1;
}

unsigned char cmos_get(unsigned char reg)
{
//Set Register
    outb(0x70,((cmos_disablenmi&0x1)<<7)|reg);

//Return value
    return inb(0x71);
}

void cmos_set(unsigned char reg,unsigned char value)
{
//Set Register
    outb(0x70,((cmos_disablenmi&0x1)<<7)|reg);

//Set value
    outb(0x71,value);
}