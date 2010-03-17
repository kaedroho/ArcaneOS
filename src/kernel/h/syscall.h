#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

#include "sys.h"

extern unsigned int syscall(unsigned int id, ...);
extern unsigned int syscall_handler(struct regs *r);
extern void syscall_receiver();

extern void syscall_init();
extern void* syscall_ptrs[];
extern unsigned int syscall_stack_sizes[];

enum syscall_id
{
    syscall_sleep_id = 0
};

extern unsigned int syscall_sleep(int c, int d, unsigned int duration, int a, int b);

#endif // SYSCALL_H_INCLUDED
