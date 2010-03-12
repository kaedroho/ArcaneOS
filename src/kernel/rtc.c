#include "sys.h"
#include "cli.h"

struct rtc_time rtc_currenttime;
unsigned char rtc_statusregisterb;

void (*rtc_clockhandler)();

unsigned char rtc_convertbcdtobinary(unsigned char bcd)
{
//Variables
    unsigned char upperbits=(bcd>>4)&0x0F;
    unsigned char lowerbits=bcd&0x0F;

//Return binary version
    return lowerbits+upperbits*10;
}

void rtc_updatefromcmos()
{
//Get time from cmos
    rtc_currenttime.seccond=cmos_get(0x00);
    rtc_currenttime.minute=cmos_get(0x02);
    rtc_currenttime.hour=cmos_get(0x04);
    rtc_currenttime.weekday=cmos_get(0x06);
    rtc_currenttime.dayofmonth=cmos_get(0x07);
    rtc_currenttime.month=cmos_get(0x08);
    rtc_currenttime.year=cmos_get(0x09);
    rtc_currenttime.century=cmos_get(0x32);

//If the 12 hour bit is set. Then mask the 0x8 bit of hour and store values in remo variable
    unsigned char twelvehourmode=0;
    unsigned char ampm=0;
    if((rtc_statusregisterb)&0x1)
    {
        twelvehourmode=1;
        ampm=(rtc_currenttime.hour>>7)&0x1;
        rtc_currenttime.hour=rtc_currenttime.hour&0x7F;
    }

//If the BCD bit is set then convert the time to binary
    if((rtc_statusregisterb>>1)&0x1)
    {
        rtc_currenttime.seccond=rtc_convertbcdtobinary(rtc_currenttime.seccond);
        rtc_currenttime.minute=rtc_convertbcdtobinary(rtc_currenttime.minute);
        rtc_currenttime.hour=rtc_convertbcdtobinary(rtc_currenttime.hour);
        rtc_currenttime.weekday=rtc_convertbcdtobinary(rtc_currenttime.weekday);
        rtc_currenttime.dayofmonth=rtc_convertbcdtobinary(rtc_currenttime.dayofmonth);
        rtc_currenttime.month=rtc_convertbcdtobinary(rtc_currenttime.month);
        rtc_currenttime.year=rtc_convertbcdtobinary(rtc_currenttime.year);
        rtc_currenttime.century=rtc_convertbcdtobinary(rtc_currenttime.century);
    }

//If its in 12 hour mode, convert the numbers into 24 hour
    if(twelvehourmode==1)
    {
        rtc_currenttime.hour+=12;
        if(rtc_currenttime.hour==24)
            rtc_currenttime.hour=0;
    }
}

void rtc_init()
{
//Get modes
    rtc_statusregisterb=cmos_get(0xb);

//Zero handler
    rtc_clockhandler=0;

//Update the time from the cmos
    rtc_updatefromcmos();
}

unsigned char rtc_getmonthend(unsigned char month,unsigned char year,unsigned char century)
{
    return 31;
}

void rtc_tickseccond()
{
//Increase seccond count
    rtc_currenttime.seccond++;

//Update from CMOS when secconds reach 30
    if(rtc_currenttime.seccond==30)
        rtc_updatefromcmos();

//Increase minutes when secconds reach 60
    if(rtc_currenttime.seccond==60)
    {
        rtc_currenttime.seccond=0;
        rtc_currenttime.minute++;

    //Increase hours when minutes reach 60
        if(rtc_currenttime.minute==60)
        {
            rtc_currenttime.minute=0;
            rtc_currenttime.hour++;

        //Increase days when hours reach 24
            if(rtc_currenttime.hour==24)
            {
                rtc_currenttime.hour=0;
                rtc_currenttime.weekday++;
                rtc_currenttime.dayofmonth++;

            //Set weekday to 0 if it is 7
                if(rtc_currenttime.weekday==8)
                    rtc_currenttime.weekday=1;

            //Increase months when days reach the months end
                if(rtc_currenttime.dayofmonth==rtc_getmonthend(rtc_currenttime.month,rtc_currenttime.year,rtc_currenttime.century))
                {
                    rtc_currenttime.dayofmonth=0;
                    rtc_currenttime.month++;

                //Increase years when months reach 12
                    if(rtc_currenttime.month==12)
                    {
                        rtc_currenttime.month=0;
                        rtc_currenttime.year++;

                    //Increase centurys when years reach 100
                        if(rtc_currenttime.year==100)
                        {
                            rtc_currenttime.century++;
                            rtc_currenttime.year=0;
                        }
                    }
                }
            }
        }
    }

//Call the clock handler
    if(rtc_clockhandler!=0)
        rtc_clockhandler();
}

struct rtc_time* rtc_gettime()
{
    return &rtc_currenttime;
}

void rtc_setclockhandler(void (*handler)())
{
    rtc_clockhandler=handler;
}
