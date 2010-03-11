#include "sys.h"
#include "cli.h"
#include "ui/ui.h"
#include "boot.h"
#include "video/video.h"
#include "mm.h"

extern void start();
extern void kernel_end();

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

    mm_init_stack_page_allocator();

    void* ptr1 = mm_page_alloc(1);
    void* ptr2 = mm_page_alloc(1);

    cli_puts("\n\nAllocated page 1: 0x"); cli_putu32((unsigned int)ptr1,16);
    cli_puts("\n\nAllocated page 2: 0x"); cli_putu32((unsigned int)ptr2,16);
    for (;;);

//Start ui
    ui_start();

//Endless loop
    for(;;);
}
