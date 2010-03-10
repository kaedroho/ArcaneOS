#include "sys.h"
#include "cli.h"
#include "ui/ui.h"
#include "boot.h"
#include "video/video.h"

int main()
{
//Start video driver
    video_init();
    video_setdriver(video_text_getdriver());

//Put loading message
    cli_puts("ArcaneOS Loading...\n");

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

//Start ui
    ui_start();

//Endless loop
    for(;;);
}
