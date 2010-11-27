#include <clock.h>
#include <console.h>

struct clock_time clock_currenttime;

void (*clock_clockhandler)();
struct clock_time* (*clock_getcurrenttime)();

void clock_init()
{
//Zero handler
    clock_clockhandler=0;

//Update the time
    if(clock_getcurrenttime!=0){
        clock_currenttime=*clock_getcurrenttime();
        console_puts_protected("CLOCK: Started. Current time is: ");
        console_putu32_protected(clock_currenttime.hour,10);
        console_putc_protected(':');
        console_putu32_protected(clock_currenttime.minute,10);
        console_putc_protected(':');
        console_putu32_protected(clock_currenttime.seccond,10);
        console_putc_protected(' ');
        console_putu32_protected(clock_currenttime.dayofmonth,10);
        console_putc_protected('/');
        console_putu32_protected(clock_currenttime.month,10);
        console_putc_protected('/');
        console_putu32_protected(clock_currenttime.century,10);
        console_putu32_protected(clock_currenttime.year,10);
        console_putc_protected('\n');
    }
}

unsigned char clock_getmonthend(unsigned char month,unsigned char year,unsigned char century)
{
    if(month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12)
        return 31;
    if(month==2)
        return (year%4==0)?29:28;
    return 30;
}

void clock_tickseccond()
{
//Increase seccond count
    clock_currenttime.seccond++;

//Update from CMOS when secconds reach 30
    if(clock_currenttime.seccond==30 && clock_getcurrenttime!=0)
        clock_currenttime=*clock_getcurrenttime();

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