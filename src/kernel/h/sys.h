#ifndef _SYSTEM_H
#define _SYSTEM_H

//SYSTEM FUNCTIONS
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port,unsigned char data);
extern unsigned short inw(unsigned short port);
extern void outw(unsigned short port,unsigned short data);
extern unsigned long inl(unsigned short port);
extern void outl(unsigned short port,unsigned long data);
extern unsigned char peekb(unsigned short seg,unsigned short off);
extern void pokeb(unsigned short seg,unsigned short off,unsigned char val);
extern unsigned short peekw(unsigned short seg,unsigned short off);
extern void pokew(unsigned short seg,unsigned short off,unsigned short val);
extern unsigned long peekl(unsigned short seg,unsigned short off);
extern void pokel(unsigned short seg,unsigned short off,unsigned long val);

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
extern void irq_remap(void);
extern void irq_unmap(void);

extern unsigned int irq_lock();
extern void irq_unlock(unsigned int handle);

//TIMER FUNCTIONS
extern void timer_init();
extern unsigned int timer_getticks();
extern unsigned int timer_getticksperseccond();

//KEYBOARD FUNCTIONS
extern void kb_init();
extern void kb_setkeyboardhandler(void (*handler)(unsigned char scancode));
extern char kb_convertscancodetoascii(unsigned char scancode);

//CPUID STRUCTURE
struct cpuid_struct
{
    unsigned int maxstandardlevel;                  //0000_0000h (EAX)
    char vendor[13];                                //0000_0000h (EBX ECX EDX)

    unsigned int processorinfoa;                    //0000_0001h (EAX)
    unsigned int processorinfob;                    //0000_0001h (EBX)
    unsigned int processorinfoc;                    //0000_0001h (ECX)
    unsigned int processorinfod;                    //0000_0001h (EDX)

    unsigned int l1cacheconfiga;                    //0000_0004h (EAX)
    unsigned int l1cacheconfigb;                    //0000_0004h (EBX)
    unsigned int l1cacheconfigc;                    //0000_0004h (ECX)
    unsigned int l1cacheconfigd;                    //0000_0004h (EDX)

    unsigned int l2cacheconfiga;                    //0000_0004h (EAX)
    unsigned int l2cacheconfigb;                    //0000_0004h (EBX)
    unsigned int l2cacheconfigc;                    //0000_0004h (ECX)
    unsigned int l2cacheconfigd;                    //0000_0004h (EDX)

    unsigned int moninfoa;                          //0000_0005h (EAX)
    unsigned int moninfob;                          //0000_0005h (EBX)
    unsigned int moninfoc;                          //0000_0005h (ECX)
    unsigned int moninfod;                          //0000_0005h (EDX)

    unsigned int powermanagementinfoa;              //0000_0006h (EAX)
    unsigned int powermanagementinfob;              //0000_0006h (EBX)
    unsigned int powermanagementinfoc;              //0000_0006h (ECX)

    unsigned int dcaparams;                         //0000_0009h (EAX)

    unsigned int architecturalpemoinfoa;            //0000_000Ah (EAX)
    unsigned int architecturalpemoinfob;            //0000_000Ah (EBX)
    unsigned int architecturalpemoinfod;            //0000_000Ah (EDX)
} __attribute__((packed));

//CPUID FUNCTIONS
extern void cpuid_init();
extern struct cpuid_struct* cpuid_getstruct();

//CMOS FUNCTIONS
extern void cmos_init();
extern unsigned char cmos_get(unsigned char reg);
extern void cmos_set(unsigned char reg,unsigned char value);

//RTC STRUCTURES
struct rtc_time
{
    unsigned char seccond;
    unsigned char minute;
    unsigned char hour;
    unsigned char weekday;
    unsigned char dayofmonth;
    unsigned char month;
    unsigned char year;
    unsigned char century;
};

//RTC FUNCTIONS
extern void rtc_init();
extern void rtc_tickseccond();
extern struct rtc_time* rtc_gettime();
extern void rtc_setclockhandler(void (*handler)());

//POWER FUNCTIONS
extern void power_init();
extern void power_shutdown();
extern void power_reboot();
#endif
