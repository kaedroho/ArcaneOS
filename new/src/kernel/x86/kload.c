#include <x86/sys.h>
#include <console.h>

extern unsigned char _low_start, _low_end;

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

    pg_init();
    mm_init();
}