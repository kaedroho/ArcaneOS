#include <sys-x86.h>
#include <clock.h>

struct clock_time clock_currenttime;
unsigned char clock_statusregisterb;

void (*clock_clockhandler)();

unsigned char clock_convertbcdtobinary(unsigned char bcd)
{
//Variables
    unsigned char upperbits=(bcd>>4)&0x0F;
    unsigned char lowerbits=bcd&0x0F;

//Return binary version
    return lowerbits+upperbits*10;
}

void clock_updatefromcmos()
{
//Get time from cmos
    clock_currenttime.seccond=cmos_get(0x00);
    clock_currenttime.minute=cmos_get(0x02);
    clock_currenttime.hour=cmos_get(0x04);
    clock_currenttime.weekday=cmos_get(0x06);
    clock_currenttime.dayofmonth=cmos_get(0x07);
    clock_currenttime.month=cmos_get(0x08);
    clock_currenttime.year=cmos_get(0x09);
    clock_currenttime.century=cmos_get(0x32);

//If the 12 hour bit is set. Then mask the 0x8 bit of hour and store values in remo variable
    unsigned char twelvehourmode=0;
    unsigned char ampm=0;
    if((clock_statusregisterb)&0x1)
    {
        twelvehourmode=1;
        ampm=(clock_currenttime.hour>>7)&0x1;
        clock_currenttime.hour=clock_currenttime.hour&0x7F;
    }

//If the BCD bit is set then convert the time to binary
    if((clock_statusregisterb>>1)&0x1)
    {
        clock_currenttime.seccond=clock_convertbcdtobinary(clock_currenttime.seccond);
        clock_currenttime.minute=clock_convertbcdtobinary(clock_currenttime.minute);
        clock_currenttime.hour=clock_convertbcdtobinary(clock_currenttime.hour);
        clock_currenttime.weekday=clock_convertbcdtobinary(clock_currenttime.weekday);
        clock_currenttime.dayofmonth=clock_convertbcdtobinary(clock_currenttime.dayofmonth);
        clock_currenttime.month=clock_convertbcdtobinary(clock_currenttime.month);
        clock_currenttime.year=clock_convertbcdtobinary(clock_currenttime.year);
        clock_currenttime.century=clock_convertbcdtobinary(clock_currenttime.century);
    }

//If its in 12 hour mode, convert the numbers into 24 hour
    if(twelvehourmode==1)
    {
        clock_currenttime.hour+=12;
        if(clock_currenttime.hour==24)
            clock_currenttime.hour=0;
    }
}

void clock_init()
{
//Get modes
    clock_statusregisterb=cmos_get(0xb);

//Zero handler
    clock_clockhandler=0;

//Update the time from the cmos
    clock_updatefromcmos();
}

unsigned char clock_getmonthend(unsigned char month,unsigned char year,unsigned char century)
{
    return 31;
}

void clock_tickseccond()
{
//Increase seccond count
    clock_currenttime.seccond++;

//Update from CMOS when secconds reach 30
    if(clock_currenttime.seccond==30)
        clock_updatefromcmos();

//Increase minutes when secconds reach 60
    if(clock_currenttime.seccond==60)
    {
        clock_currenttime.seccond=0;
        clock_currenttime.minute++;

    //Increase hours when minutes reach 60
        if(clock_currenttime.minute==60)
        {
            clock_currenttime.minute=0;
            clock_currenttime.hour++;

        //Increase days when hours reach 24
            if(clock_currenttime.hour==24)
            {
                clock_currenttime.hour=0;
                clock_currenttime.weekday++;
                clock_currenttime.dayofmonth++;

            //Set weekday to 0 if it is 7
                if(clock_currenttime.weekday==8)
                    clock_currenttime.weekday=1;

            //Increase months when days reach the months end
                if(clock_currenttime.dayofmonth==clock_getmonthend(clock_currenttime.month,clock_currenttime.year,clock_currenttime.century))
                {
                    clock_currenttime.dayofmonth=0;
                    clock_currenttime.month++;

                //Increase years when months reach 12
                    if(clock_currenttime.month==12)
                    {
                        clock_currenttime.month=0;
                        clock_currenttime.year++;

                    //Increase centurys when years reach 100
                        if(clock_currenttime.year==100)
                        {
                            clock_currenttime.century++;
                            clock_currenttime.year=0;
                        }
                    }
                }
            }
        }
    }

//Call the clock handler
    if(clock_clockhandler!=0)
        clock_clockhandler();
}

struct clock_time* clock_gettime()
{
    return &clock_currenttime;
}

void clock_setclockhandler(void (*handler)())
{
    clock_clockhandler=handler;
}