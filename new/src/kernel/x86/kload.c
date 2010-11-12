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

    // VBE Test
    int mode = 0;
    vbe_get_mode(&mode);
    console_puts_protected("VBE Mode: ");
    console_puti32(mode, 10);
    console_puts_protected("\n");

}