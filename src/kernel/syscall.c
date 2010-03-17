#include "syscall.h"
#include "mt.h"

void* syscall_ptrs[] = {
    &syscall_sleep
    };

unsigned int syscall_stack_sizes[] = {
    4
    };

unsigned int syscall_cur_stack_size;
void* syscall_cur_ptr;
unsigned int syscall_return_value;
struct regs* syscall_regs;

void syscall_init()
{
    idt_set_gate(0x80,(unsigned)syscall_receiver,0x08,0x8E);
}

unsigned int syscall_handler(struct regs *r)
{
    unsigned int id = r->eax;
    syscall_cur_stack_size = syscall_stack_sizes[id];
    syscall_cur_ptr = syscall_ptrs[id];
    unsigned char* esp1 = (unsigned char*)r->esp + syscall_cur_stack_size + 4;
    unsigned char* esp2;
    unsigned int i;

    __asm__ __volatile__ ("mov %%esp,%0" : "=a" (esp2));

    syscall_regs = r;
    for (i = 0; i < syscall_cur_stack_size; i++)
        *--esp2 = *--esp1;

    __asm__ __volatile__ ("sub %%esp,%0" : : "a" (syscall_cur_stack_size));
    __asm__ __volatile__ ("mov %0, %%eax" : : "a" (syscall_cur_ptr));
    __asm__ __volatile__ ("call *%eax");
    __asm__ __volatile__ ("mov %%eax, %0" : "=a" (syscall_return_value));
    __asm__ __volatile__ ("add %%esp,%0" : : "a" (syscall_cur_stack_size));

    return syscall_return_value;
}

unsigned int syscall_sleep(int c, int d, unsigned int duration, int a, int b)
{
    cli_puts("A: "); cli_putu32(a,10); cli_puts("\n");
    cli_puts("B: "); cli_putu32(b,10); cli_puts("\n");
    cli_puts("C: "); cli_putu32(c,10); cli_puts("\n");
    cli_puts("D: "); cli_putu32(d,10); cli_puts("\n");
    cli_puts("Duration: "); cli_putu32(duration,10); cli_puts("\n");
    mt_sleep(mt_first_process->first_thread, duration);
    mt_switch(syscall_regs);
    return 0;
}
