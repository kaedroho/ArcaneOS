#ifndef _SYSTEM_H
#define _SYSTEM_H

//SYSTEM FUNCTIONS
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port,unsigned char data);

//GDT FUNCTIONS
extern void gdt_init();
extern void gdt_set_gate(int num,unsigned long base,unsigned long limit,unsigned char access,unsigned char gran);

//IDT FUNCTIONS
extern void idt_init();
extern void idt_set_gate(unsigned char num,unsigned long base,unsigned short sel,unsigned char flags);

//REGISTER STRUCTURE (FOR USE WITH INTERRUPTS)
struct regs
{
    unsigned int gs,fs,es,ds;
    unsigned int edi,esi,ebp,esp,ebx,edx,ecx,eax;
    unsigned int int_no,err_code;
    unsigned int eip,cs,eflags,useresp,ss;
};

//ISR FUNCTIONS
extern void isr_init();

//IRQ FUNCTIONS
extern void irq_init();
extern void irq_install_handler(int irq,void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);

//TIMER FUNCTIONS
extern void timer_init();

//KEYBOARD FUNCTIONS
extern void kb_init();
extern void kb_setkeyboardhandler(void (*handler)(unsigned char scancode));
extern char kb_convertscancodetoascii(unsigned char scancode);

#endif
