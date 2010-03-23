#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

#include "sys.h"

#define SYSCALL_COUNT 256

struct mt_process;
struct mt_thread;

extern unsigned int syscall(unsigned int id, ...);
extern void syscall_handler(struct regs *r);
extern void syscall_receiver();

extern void syscall_init();
extern void* syscall_ptrs[SYSCALL_COUNT];
extern unsigned int syscall_stack_sizes[SYSCALL_COUNT];
extern void syscall_register(unsigned int id, void* function_ptr, unsigned int parameter_bytes);

enum syscall_id
{
    syscall_sleep_id = 0,
    syscall_floppy_id,
    syscall_create_thread_id,
    syscall_end_thread_id
};

// INTERNAL
extern void _syscall_sleep(unsigned int duration);
extern void _syscall_floppy(unsigned int id, unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, unsigned char* buffer, unsigned int* error);
extern struct mt_thread* _syscall_create_thread(struct mt_process* process, void* eip, int stack_pages);
extern void _syscall_end_thread();

// EXTERNAL
extern void syscall_sleep(unsigned int duration);
extern void syscall_floppy_read(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, unsigned char* buffer, unsigned int* error);
extern void syscall_floppy_write(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int offset, unsigned int size, const unsigned char* buffer, unsigned int* error);
extern void syscall_floppy_flush(unsigned int* error);
extern struct mt_thread* syscall_create_thread(struct mt_process* process, void* eip, int stack_pages);
extern void syscall_end_thread();

#endif // SYSCALL_H_INCLUDED
