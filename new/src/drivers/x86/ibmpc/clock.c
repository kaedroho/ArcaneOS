#include <x86/sys.h>
#include <clock.h>

unsigned char clock_statusregisterb;
struct clock_time clock_temptime;

extern struct clock_time* (*clock_getcurrenttime)();

unsigned char clock_convertbcdtobinary(unsigned char bcd)
{
//Variables
    unsigned char upperbits=(bcd>>4)&0x0F;
    unsigned char lowerbits=bcd&0x0F;

//Return binary version
    return lowerbits+upperbits*10;
}

struct clock_time* clock_updatefromcmos()
{
//Get time from cmos
    clock_temptime.seccond=cmos_get(0x00);
    clock_temptime.minute=cmos_get(0x02);
    clock_temptime.hour=cmos_get(0x04);
    clock_temptime.weekday=cmos_get(0x06);
    clock_temptime.dayofmonth=cmos_get(0x07);
    clock_temptime.month=cmos_get(0x08);
    clock_temptime.year=cmos_get(0x09);
    clock_temptime.century=cmos_get(0x32);

//If the 12 hour bit is set. Then mask the 0x8 bit of hour and store values in remo variable
    unsigned char twelvehourmode=0;
    unsigned char ampm=0;
    if((clock_statusregisterb)&0x1)
    {
        twelvehourmode=1;
        ampm=(clock_temptime.hour>>7)&0x1;
        clock_temptime.hour=clock_temptime.hour&0x7F;
    }

//If the BCD bit is set then convert the time to binary
    if((clock_statusregisterb>>1)&0x1)
    {
        clock_temptime.seccond=clock_convertbcdtobinary(clock_temptime.seccond);
        clock_temptime.minute=clock_convertbcdtobinary(clock_temptime.minute);
        clock_temptime.hour=clock_convertbcdtobinary(clock_temptime.hour);
        clock_temptime.weekday=clock_convertbcdtobinary(clock_temptime.weekday);
        clock_temptime.dayofmonth=clock_convertbcdtobinary(clock_temptime.dayofmonth);
        clock_temptime.month=clock_convertbcdtobinary(clock_temptime.month);
        clock_temptime.year=clock_convertbcdtobinary(clock_temptime.year);
        clock_temptime.century=clock_convertbcdtobinary(clock_temptime.century);
    }

//If its in 12 hour mode, convert the numbers into 24 hour
    if(twelvehourmode==1)
    {
        clock_temptime.hour+=12;
        if(clock_temptime.hour==24)
            clock_temptime.hour=0;
    }

    return &clock_temptime;
}


void ibmpc_initclock()
{
//Get status register b
    clock_statusregisterb=cmos_get(0xb);

//Get current time callback
    clock_getcurrenttime=clock_updatefromcmos;
}