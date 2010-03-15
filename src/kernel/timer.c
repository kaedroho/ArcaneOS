#include "sys.h"
#include "cli.h"
#include "mt.h"

unsigned int timer_ticks = 0;
unsigned int timer_ticksperseccond;

void timer_handler(struct regs *r)
{
 //Increase tick count
    timer_ticks++;

//If ticks are a factor of 1000 then 1 seccond has passed
    if(timer_ticks%timer_ticksperseccond==0)
        rtc_tickseccond();

    // Context switch
    if (timer_ticks%50==0)
        mt_switch(r);
}

void timer_phase(int hz)
{
//Set ticks per seccond
    timer_ticksperseccond=hz;

//Calculate devisor
    int divisor=1193180/hz;

//Set control byte
    outb(0x43,0x36);

//Send divisor
    outb(0x40,divisor&0xFF);
    outb(0x40,divisor>>8);
}


void timer_init()
{
//Install timer handler
    irq_install_handler(0,timer_handler);

//Set ticks per seccond to 1000
    timer_phase(1000);
}
