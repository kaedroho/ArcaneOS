#include "sys.h"

struct rtc_time rtc_currenttime;

void rtc_updatefromcmos()
{

}

void rtc_init()
{
//Update the time from the cmos
    rtc_updatefromcmos();
}

unsigned char rtc_getmonthend(unsigned char month,unsigned char year,unsigned char century)
{

}

void rtc_tickseccond()
{
//Increase seccond count
    rtc_currenttime.seccond++;

//Increase minutes when secconds reach 60
    if(rtc_currenttime.seccond==60)
    {
        rtc_currenttime.seccond=0;
        rtc_currenttime.minute++;

    //Update from cmos when minute is 30
        if(rtc_currenttime.minute==30)
            rtc_updatefromcmos();

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
                if(rtc_currenttime.weekday==7)
                    rtc_currenttime.weekday=0;

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
}
