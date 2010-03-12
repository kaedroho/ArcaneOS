#include "sys.h"
#include "cli.h"
#include "ui/ui.h"
#include "boot.h"
#include "video/video.h"
#include "mm.h"
#include "paging.h"
#include "acpi.h"

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
    cmos_init();
    rtc_init();
    acpi_init();
    power_init();

//Start interrupts
    __asm__ __volatile__ ("sti");

    void* ptr1 = mm_page_alloc();
    void* ptr2 = mm_page_alloc();

    unsigned int* block_ptr = (unsigned int*)mm_block_alloc(16);
    block_ptr[0] = 1;
    block_ptr[1] = 2;
    block_ptr[2] = 3;
    block_ptr[3] = 4;

    mm_page_free(ptr1);

    //*(unsigned int*)ptr1 = 1; // UNCOMMENT THIS LINE TO CAUSE A PAGE FAULT!
    *(unsigned int*)ptr2 = 1;

    struct cpuid_struct* cpuidstruct=cpuid_getstruct();
    cli_puts("Vendor: ");cli_puts(cpuidstruct->vendor);cli_putch('\n');

    cli_puts("\n\nCurrent page directory: 0x"); cli_putu32((unsigned int)pg_get_directory(),16);
    cli_puts("\n\nKernel page directory: 0x"); cli_putu32((unsigned int)pg_kernel_directory,16);

    cli_puts("\n\nAllocated page 1: 0x"); cli_putu32((unsigned int)ptr1,16);
    cli_puts("\n\nAllocated page 2: 0x"); cli_putu32((unsigned int)ptr2,16);

    cli_puts("\n\nAllocated block: 0x"); cli_putu32((unsigned int)block_ptr,16);

    if (pg_get_enabled())
        cli_puts("\n\nPaging is enabled!");

    for (;;);

//Start ui
    ui_start();

//Endless loop
    for(;;);
}
