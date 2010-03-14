#include "mt.h"
#include "mm.h"
#include "libs/memory.h"

extern void gdt_setup();

struct mt_tss mt_task_state_segment;
unsigned int mt_syscall_stack;
struct mt_process* mt_first_process;
struct mt_process* mt_last_process;

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
}

struct mt_thread* mt_create_thread(struct mt_process* process, void* eip, int stack_pages)
{
    struct mt_thread* thread = (struct mt_thread*)mm_block_alloc(sizeof(struct mt_thread));
    thread->stack_base = (unsigned int)mm_page_alloc(process->directory,stack_pages) + mm_page_size*stack_pages;
    thread->state.esp = mt_syscall_stack;
    thread->state.useresp = thread->stack_base;
    thread->state.eip = (unsigned int)eip;

    thread->prev_thread = process->last_thread;
    thread->next_thread = 0;
    if (process->last_thread)
        process->last_thread->next_thread = thread;
    else
        process->first_thread = thread;
    process->last_thread = thread;

    return thread;
}
struct mt_process* mt_create_process(void* eip, struct pg_directory* directory, int stack_pages)
{
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

    return process;
}
