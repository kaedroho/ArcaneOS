#include "sys.h"
#include "cli.h"
#include "ui/ui.h"
#include "boot.h"
#include "video/video.h"
#include "mm.h"
#include "paging.h"

extern void start();
extern void kernel_end();

int main()
{
//Start video driver (must always be before loading message)
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
    mm_init(); // Must be called before pg_init()
    pg_init();
    cpuid_init();

//Start interrupts
    __asm__ __volatile__ ("sti");

    void* ptr1 = mm_page_alloc();
    void* ptr2 = mm_page_alloc();

    mm_page_free(ptr1);

    *(unsigned int*)ptr1 = 1;

    struct cpuid_struct* cpuidstruct=cpuid_getstruct();
    cli_puts("Vendor: ");cli_puts(cpuidstruct->vendor);cli_putch('\n');

    cli_puts("\n\nCurrent page directory: 0x"); cli_putu32((unsigned int)pg_get_directory(),16);
    cli_puts("\n\nKernel page directory: 0x"); cli_putu32((unsigned int)pg_kernel_directory,16);

    cli_puts("\n\nAllocated page 1: 0x"); cli_putu32((unsigned int)ptr1,16);
    cli_puts("\n\nAllocated page 2: 0x"); cli_putu32((unsigned int)ptr2,16);
    //for (;;);

//Start ui
    ui_start();

//Endless loop
    for(;;);
}
