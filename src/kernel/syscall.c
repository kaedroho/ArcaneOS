#include "syscall.h"
#include "mt.h"

void* syscall_ptrs[SYSCALL_COUNT];
unsigned int syscall_stack_sizes[SYSCALL_COUNT];

unsigned int syscall_cur_stack_size;
void* syscall_cur_ptr;
unsigned int syscall_return_value;
struct regs* syscall_regs;

void syscall_init()
{
    idt_set_gate(0x80,(unsigned)syscall_receiver,0x08,0x8E);

    syscall_register(syscall_sleep_id, syscall_sleep, 4);
}

void syscall_register(unsigned int id, void* function_ptr, unsigned int parameter_bytes)
{
    syscall_ptrs[id] = function_ptr;
    syscall_stack_sizes[id] = parameter_bytes;
}

void syscall_handler(struct regs *r)
{
    unsigned int id = r->eax;
    syscall_cur_stack_size = syscall_stack_sizes[id];
    syscall_cur_ptr = syscall_ptrs[id];
    unsigned char* esp1 = (unsigned char*)r->esp + syscall_cur_stack_size + 7*4;
    unsigned char* esp2;
    unsigned int i;

    __asm__ __volatile__ ("mov %%esp,%0" : "=a" (esp2));

    syscall_regs = r;
    for (i = 0; i < syscall_cur_stack_size; i++)
        *--esp2 = *--esp1;

    __asm__ __volatile__ ("sub %0,%%esp" : : "a" (syscall_cur_stack_size));
    __asm__ __volatile__ ("mov %0, %%eax" : : "a" (syscall_cur_ptr));
    __asm__ __volatile__ ("call *%eax");
    __asm__ __volatile__ ("mov %%eax, %0" : "=a" (syscall_return_value));
    __asm__ __volatile__ ("add %0,%%esp" : : "a" (syscall_cur_stack_size));

    r->eax = syscall_return_value;
}

void syscall_sleep(unsigned int duration)
{
    mt_sleep(mt_first_process->first_thread, duration);
    mt_switch(syscall_regs);
}
