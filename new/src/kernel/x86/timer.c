#include <x86/sys.h>
#include <x86/mt.h>
#include <clock.h>
#include <console.h>

unsigned int timer_ticks = 0;
unsigned int timer_ticksperseccond;

void timer_handler(struct regs *r)
{
 //Increase tick count
    timer_ticks++;

    syscall(&mt_tick, 1000/timer_ticksperseccond);
    syscall(&mt_schedule);

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

//Set ticks per seccond to 100
    timer_phase(100);

//Print message
    console_puts_protected("TIMER: Started. Running at ");
    console_putu32_protected(timer_ticksperseccond,10);
    console_puts_protected("Hz.\n");
}