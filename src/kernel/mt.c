#include "mt.h"
#include "mm.h"
#include "libs/memory.h"
#include "libs/rect.h"
#include "paging.h"

extern void gdt_setup();

struct mt_tss mt_task_state_segment;
unsigned int mt_syscall_stack;
unsigned int mt_thread_stack_ptr;
struct mt_process* mt_first_process;
struct mt_process* mt_last_process;
struct mt_process* mt_kernel_process;

int mt_has_started = 0;
unsigned int mt_context_switch_delay = 50;

void mt_init()
{
    unsigned int stack_size = 2;

    // Allocate a stack for system calls
    mt_syscall_stack = (unsigned int)mm_page_kalloc(stack_size) + mm_page_size*stack_size;


    memset((unsigned char*)&mt_task_state_segment,0,sizeof(mt_task_state_segment));

    mt_task_state_segment.esp0 = mt_syscall_stack;
    mt_task_state_segment.ss0 = 2 << 3;
    mt_task_state_segment.iomap = sizeof(mt_task_state_segment);

    __asm__ __volatile__ ( "ltr %%ax" : : "a" ( 5 << 3 ) );

    // Create the main kernel process
    mt_kernel_process = (struct mt_process*)mm_block_alloc(sizeof(struct mt_process));
    mt_kernel_process->directory = pg_kernel_directory;

    mt_kernel_process->prev_process = 0;
    mt_kernel_process->next_process = 0;

    mt_first_process = mt_kernel_process;
    mt_last_process = mt_kernel_process;

    // Create the main kernel thread
    struct mt_thread* thread = (struct mt_thread*)mm_block_alloc(sizeof(struct mt_thread));
    thread->stack_base = (unsigned int)mm_page_alloc(mt_kernel_process->directory,stack_size) + mm_page_size*stack_size;
    thread->stack_ptr = thread->stack_base;

    thread->prev_thread = 0;
    thread->next_thread = 0;

    thread->process = mt_kernel_process;

    mt_kernel_process->first_thread = thread;
    mt_kernel_process->last_thread = thread;

}

struct mt_thread* mt_create_thread(struct mt_process* process, void* eip, int stack_pages)
{
    unsigned int handle = irq_lock();

    struct mt_thread* thread = (struct mt_thread*)mm_block_alloc(sizeof(struct mt_thread));

    thread->stack_base = (unsigned int)mm_page_alloc(process->directory,stack_pages) + mm_page_size*stack_pages;

    unsigned int* stack_ptr = (unsigned int*)thread->stack_base;

    *--stack_ptr = 2 << 3;
    *--stack_ptr = 0;
    *--stack_ptr = 0x202;
    *--stack_ptr = 1 << 3;
    *--stack_ptr = (unsigned int)eip;
    *--stack_ptr = 0;
    *--stack_ptr = 0x20;
    *--stack_ptr = 0;
    *--stack_ptr = 0;
    *--stack_ptr = 0;
    *--stack_ptr = 0;
    *--stack_ptr = 0;
    *--stack_ptr = thread->stack_base;
    *--stack_ptr = 0;
    *--stack_ptr = 0;
    *--stack_ptr = 2 << 3;
    *--stack_ptr = 2 << 3;
    *--stack_ptr = 2 << 3;
    *--stack_ptr = 2 << 3;

    thread->stack_ptr = (unsigned int)stack_ptr;

    thread->prev_thread = process->last_thread;
    thread->next_thread = 0;
    if (process->last_thread)
        process->last_thread->next_thread = thread;
    else
        process->first_thread = thread;
    process->last_thread = thread;

    thread->process = process;

    irq_unlock(handle);

    return thread;
}
struct mt_process* mt_create_process(void* eip, struct pg_directory* directory, int stack_pages)
{
    unsigned int handle = irq_lock();

    struct mt_process* process = (struct mt_process*)mm_block_alloc(sizeof(struct mt_process));
    process->directory = directory;
    process->first_thread = 0;
    process->last_thread = 0;

    process->prev_process = mt_last_process;
    process->next_process = 0;
    if (mt_last_process)
        mt_last_process->next_process = process;
    else
        mt_first_process = process;
    mt_last_process = process;

    mt_create_thread(process,eip,stack_pages);

    irq_unlock(handle);

    return process;
}
void mt_switch(struct regs* state)
{
    struct mt_process* process = mt_first_process;
    struct mt_thread* thread = process->first_thread;
    struct mt_thread* old_thread = thread;

    do
    {
        if (process->next_process)
        {
            mt_first_process = mt_first_process->next_process;
            process->next_process = 0;
            mt_first_process->prev_process = 0;

            mt_last_process->next_process = process;
            process->prev_process = mt_last_process;
            mt_last_process = process;

            process = mt_first_process;
        }

        thread = process->first_thread;
        if (thread->next_thread)
        {
            process->first_thread = thread->next_thread;
            thread->next_thread = 0;
            process->first_thread->prev_thread = 0;

            process->last_thread->next_thread = thread;
            thread->prev_thread = process->last_thread;
            process->last_thread = thread;

            thread = process->first_thread;
        }

    } while (thread->sleep_time);

    if (thread != old_thread)
    {
        old_thread->stack_ptr = mt_thread_stack_ptr;
        pg_set_directory(thread->process->directory);
        mt_thread_stack_ptr = thread->stack_ptr;
    }

    for (process = mt_first_process; process; process = process->next_process)
        for (thread = process->first_thread; thread; thread = thread->next_thread)
            if (thread->sleep_time)
                thread->sleep_time -= mt_context_switch_delay;
}
extern void irq_raise(unsigned int i);

void mt_sleep(struct mt_thread* thread, unsigned int duration)
{
    duration = (duration / mt_context_switch_delay)*mt_context_switch_delay;

    unsigned int handle = irq_lock();

    if (thread->sleep_time < duration)
        thread->sleep_time = duration;

    irq_unlock(handle);
}
