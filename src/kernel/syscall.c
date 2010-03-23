#include "syscall.h"
#include "mt.h"
#include "floppy.h"

void* syscall_ptrs[SYSCALL_COUNT];
unsigned int syscall_stack_sizes[SYSCALL_COUNT];

unsigned int syscall_cur_stack_size;
void* syscall_cur_ptr;
unsigned int syscall_return_value;
struct regs* syscall_regs;

void syscall_init()
{
    idt_set_gate(0x80,(unsigned)syscall_receiver,0x08,0x8E);

    syscall_register(syscall_sleep_id, _syscall_sleep, 4);
    syscall_register(syscall_floppy_id, _syscall_floppy, 32);
    syscall_register(syscall_create_thread_id, _syscall_create_thread, 12);
    syscall_register(syscall_end_thread_id, _syscall_end_thread, 0);
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

// THESE SHOULD NOT BE CALLED DIRECTLY
void _syscall_sleep(unsigned int duration)
{
    mt_sleep(mt_first_process->first_thread, duration);
    mt_switch(syscall_regs);
}
void _syscall_floppy(unsigned int id, unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, unsigned char* buffer, unsigned int* error)
{
    floppy_push_command(id, cylinder, head, sector, offset, size, buffer, error);
}
struct mt_thread* _syscall_create_thread(struct mt_process* process, void* eip, int stack_pages)
{
    return mt_create_thread(process, eip, stack_pages);
}
void _syscall_end_thread()
{
    struct mt_thread* thread = mt_first_process->first_thread;
    mt_switch(syscall_regs);
    mt_destroy_thread(thread);
}

// THESE SHOULD BE CALLED DIRECTLY
void syscall_sleep(unsigned int duration)
{
    syscall(syscall_sleep_id, duration);
}
void syscall_floppy_read(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, unsigned char* buffer, unsigned int* error)
{
    syscall(syscall_floppy_id, floppy_command_read, cylinder, head, sector, offset, size, buffer, error);
}
void syscall_floppy_write(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, const unsigned char* buffer, unsigned int* error)
{
    syscall(syscall_floppy_id, floppy_command_write, cylinder, head, sector, offset, size, (unsigned char*)buffer, error);
}
void syscall_floppy_flush(unsigned int* error)
{
    syscall(syscall_floppy_id, floppy_command_flush, 0, 0, 0, 0, 0, 0, error);
}
struct mt_thread* syscall_create_thread(struct mt_process* process, void* eip, int stack_pages)
{
    return (struct mt_thread*)syscall(syscall_create_thread_id, process, eip, stack_pages);
}
void syscall_end_thread()
{
    syscall(syscall_end_thread_id);
}
