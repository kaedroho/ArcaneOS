#include <x86/sys.h>
#include <console.h>

//Functions from isr.asm
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();


void isr_init()
{
    idt_set_gate(0,(unsigned)isr0,0x08,0x8E);
    idt_set_gate(1,(unsigned)isr1,0x08,0x8E);
    idt_set_gate(2,(unsigned)isr2,0x08,0x8E);
    idt_set_gate(3,(unsigned)isr3,0x08,0x8E);
    idt_set_gate(4,(unsigned)isr4,0x08,0x8E);
    idt_set_gate(5,(unsigned)isr5,0x08,0x8E);
    idt_set_gate(6,(unsigned)isr6,0x08,0x8E);
    idt_set_gate(7,(unsigned)isr7,0x08,0x8E);
    idt_set_gate(8,(unsigned)isr8,0x08,0x8E);
    idt_set_gate(9,(unsigned)isr9,0x08,0x8E);
    idt_set_gate(10,(unsigned)isr10,0x08,0x8E);
    idt_set_gate(11,(unsigned)isr11,0x08,0x8E);
    idt_set_gate(12,(unsigned)isr12,0x08,0x8E);
    idt_set_gate(13,(unsigned)isr13,0x08,0x8E);
    idt_set_gate(14,(unsigned)isr14,0x08,0x8E);
    idt_set_gate(15,(unsigned)isr15,0x08,0x8E);
    idt_set_gate(16,(unsigned)isr16,0x08,0x8E);
    idt_set_gate(17,(unsigned)isr17,0x08,0x8E);
    idt_set_gate(18,(unsigned)isr18,0x08,0x8E);
    idt_set_gate(19,(unsigned)isr19,0x08,0x8E);
    idt_set_gate(20,(unsigned)isr20,0x08,0x8E);
    idt_set_gate(21,(unsigned)isr21,0x08,0x8E);
    idt_set_gate(22,(unsigned)isr22,0x08,0x8E);
    idt_set_gate(23,(unsigned)isr23,0x08,0x8E);
    idt_set_gate(24,(unsigned)isr24,0x08,0x8E);
    idt_set_gate(25,(unsigned)isr25,0x08,0x8E);
    idt_set_gate(26,(unsigned)isr26,0x08,0x8E);
    idt_set_gate(27,(unsigned)isr27,0x08,0x8E);
    idt_set_gate(28,(unsigned)isr28,0x08,0x8E);
    idt_set_gate(29,(unsigned)isr29,0x08,0x8E);
    idt_set_gate(30,(unsigned)isr30,0x08,0x8E);
    idt_set_gate(31,(unsigned)isr31,0x08,0x8E);
}

char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void exception_handler(struct regs *r)
{
//Check if it is an exception
    if (r->int_no < 32)
    {

    /*
    //Red screen of death!
        video_setdriver(video_vgatext_getdriver(),0);
        cli_setrect(0);
        cli_settextcolour(0x4F);cli_cls();
        cli_positioncursor(0,0);
        cli_puts("An exception has occurred.\nException: (");
        cli_putu32(r->int_no, 10);
        cli_puts(") - ");
        cli_puts(exception_messages[r->int_no]);
        cli_puts("\n\nSystem Halted.\n\n");

        //Put error code
        cli_puts("Technical Information:\n\nError Code:  ");
        cli_putu32(r->err_code,16);

        //Put registers
        cli_puts("\nRegs:        EAX: ");cli_putu32(r->eax,16);
        cli_puts("\n             EBX: ");cli_putu32(r->ebx,16);
        cli_puts("\n             ECX: ");cli_putu32(r->ecx,16);
        cli_puts("\n             EDX: ");cli_putu32(r->edx,16);

        cli_puts("\n             ESI: ");cli_putu32(r->esi,16);
        cli_puts("\n             EDI: ");cli_putu32(r->edi,16);

        cli_puts("\n             ESP: ");cli_putu32(r->esp,16);
        cli_puts("\n             EBP: ");cli_putu32(r->ebp,16);

        cli_puts("\n              CS: ");cli_putu32(r->cs,16);
        cli_puts("\n              DS: ");cli_putu32(r->ds,16);
        cli_puts("\n              ES: ");cli_putu32(r->es,16);
        cli_puts("\n              FS: ");cli_putu32(r->fs,16);
        cli_puts("\n              GS: ");cli_putu32(r->gs,16);
        cli_puts("\n              SS: ");cli_putu32(r->ss,16);

        cli_puts("\n             EIP: ");cli_putu32(r->eip,16);
	*/
        //Endless loop
        console_puts_protected("An exception has occurred.\nException: (0x");
        console_putu32_protected(r->err_code, 16);
        console_puts_protected(") - ");
        console_puts_protected(exception_messages[r->int_no]);
        console_puts_protected("\n\nSystem Halted.\n\n");

        //Put error code
        console_puts_protected("Technical Information:\n\nError Code:  ");
        console_putu32_protected(r->err_code,16);

        //Put registers
        console_puts_protected("\nRegs:        EAX: 0x");console_putu32_protected(r->eax,16);
        console_puts_protected("\n             EBX: 0x");console_putu32_protected(r->ebx,16);
        console_puts_protected("\n             ECX: 0x");console_putu32_protected(r->ecx,16);
        console_puts_protected("\n             EDX: 0x");console_putu32_protected(r->edx,16);

        console_puts_protected("\n             ESI: 0x");console_putu32_protected(r->esi,16);
        console_puts_protected("\n             EDI: 0x");console_putu32_protected(r->edi,16);

        console_puts_protected("\n             ESP: 0x");console_putu32_protected(r->esp,16);
        console_puts_protected("\n             EBP: 0x");console_putu32_protected(r->ebp,16);

        console_puts_protected("\n              CS: 0x");console_putu32_protected(r->cs,16);
        console_puts_protected("\n              DS: 0x");console_putu32_protected(r->ds,16);
        console_puts_protected("\n              ES: 0x");console_putu32_protected(r->es,16);
        console_puts_protected("\n              FS: 0x");console_putu32_protected(r->fs,16);
        console_puts_protected("\n              GS: 0x");console_putu32_protected(r->gs,16);
        console_puts_protected("\n              SS: 0x");console_putu32_protected(r->ss,16);

        console_puts_protected("\n             EIP: 0x");console_putu32_protected(r->eip,16);

        for(;;)
            __asm__ __volatile__ ("hlt");
    }
}