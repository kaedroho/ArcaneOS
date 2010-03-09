#include "sys.h"
#include "cli.h"
#include "ui/ui.h"

void main()
{
//Put loading message
    cli_puts("KdOS Loading...");

//Setup kernel
    gdt_init();
    idt_init();
    isr_init();
    irq_init();
    timer_init();
    kb_init();
    ui_init();

//Start interrupts
    __asm__ __volatile__ ("sti");

    cli_positioncursor(0,20);

//Start ui
    ui_start();

//Endless loop
    for(;;);
}
