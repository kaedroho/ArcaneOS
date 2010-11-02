#include <sys.h>
#include <clock.h>

unsigned int timer_ticks = 0;
unsigned int timer_ticksperseccond;

void timer_handler(struct regs *r)
{
 //Increase tick count
    timer_ticks++;

//If ticks are a factor of 1000 then 1 seccond has passed
    if(timer_ticks%timer_ticksperseccond==0)
        clock_tickseccond();
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

unsigned int timer_getticks()
{
    return timer_ticks;
}

unsigned int timer_getticksperseccond()
{
    return timer_ticksperseccond;
}

void timer_init()
{
//Install timer handler
    irq_install_handler(0,timer_handler);

//Set ticks per seccond to 1000
    timer_phase(1000);
}