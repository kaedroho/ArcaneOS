#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

#include "sys.h"

#define SYSCALL_COUNT 256

extern unsigned int syscall(unsigned int id, ...);
extern void syscall_handler(struct regs *r);
extern void syscall_receiver();

extern void syscall_init();
extern void* syscall_ptrs[SYSCALL_COUNT];
extern unsigned int syscall_stack_sizes[SYSCALL_COUNT];
extern void syscall_register(unsigned int id, void* function_ptr, unsigned int parameter_bytes);

enum syscall_id
{
    syscall_sleep_id = 0
};

extern void syscall_sleep(unsigned int duration);

#endif // SYSCALL_H_INCLUDED
