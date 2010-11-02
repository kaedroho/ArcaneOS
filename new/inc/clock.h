#ifndef CLOCK_H
#define	CLOCK_H

struct clock_time
{
    unsigned char seccond;
    unsigned char minute;
    unsigned char hour;
    unsigned char weekday;
    unsigned char dayofmonth;
    unsigned char month;
    unsigned char year;
    unsigned char century;
};

//RTC FUNCTIONS
extern void clock_init();
extern void clock_tickseccond();
extern struct clock_time* clock_gettime();
extern void clock_setclockhandler(void (*handler)());

#endif	/* CLOCK_H */

