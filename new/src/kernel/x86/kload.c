#include <x86/sys.h>
#include <console.h>
#include <string.h>
#include <x86/vbe_real.h>
#include <x86/gdt.h>

void mm_init();

void kload()
{
    console_puts_protected("KERNEL: Loading: GDT");
    gdt_init();
    console_puts_protected(" IDT");
    idt_init();
    console_puts_protected(" ISR");
    isr_init();
    console_puts_protected(" IRQ");
    irq_init();
    console_puts_protected(" CPUID");
    cpuid_init();
    console_puts_protected(" [Finished]\n");

    lm_init();
    
    pg_init();
    //pg_enablepaging();
    mm_init();

    //     .=============================================.
    //     |                   VBE Test                  |
    //     `=============================================`
#define VBE_TEST
#ifdef VBE_TEST
    struct vbe_info_block* info = (struct vbe_info_block*)0x80000;
    vbe_get_controller_info(info);
    console_puts_protected("VBE Modes: ");
    unsigned short* modes = info->video_mode_ptr;

    while (*modes != 0xFFFF) {
        console_puts_protected("0x");
        console_putu32_protected(*(modes++), 16);
        console_puts_protected(" ");
    }
    console_puts_protected("\n");

    vbe_set_mode(0xC115, 0);

    struct vbe_mode_info_block* modeinfo = (struct vbe_mode_info_block*)0x80000;
    vbe_get_mode_info(0x4115, modeinfo);

    int x, y, i;
    for (i = -200; i < 600; i++) {
        unsigned char* addr = (unsigned char*)modeinfo->phys_base_addr;
        for (y = 0; y < 600; y++)
            for (x = 0; x < 800; x++)
            {
                // Red
                *(addr++) = 0x00;
                *(addr++) = 0x00;
                *(addr++) = i+y/5 < 0 ? 0 : i+y/5 > 255 ? 255 : i+y/5;
            }
        for (y = 0; y < 1000000; y++)
            x=y;
    }
#endif
    //     .=============================================.
    //     |               End of VBE Test               |
    //     `=============================================`
}