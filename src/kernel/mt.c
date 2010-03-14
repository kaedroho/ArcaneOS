#include "mt.h"
#include "mm.h"
#include "libs/memory.h"

extern void gdt_setup();

struct mt_tss mt_task_state_segment;
unsigned int mt_syscall_stack;


void mt_init()
{
    unsigned int stack_size = 2;

    // Allocate a stack for system calls
    mt_syscall_stack = (unsigned int)mm_page_alloc(stack_size) + mm_page_size*stack_size;


    memset((unsigned char*)&mt_task_state_segment,0,sizeof(mt_task_state_segment));

    __asm__ __volatile__ ( "ltr %%ax" : : "a" ( 3 << 3 ) );
    gdt_setup();

    mt_task_state_segment.esp0 = mt_syscall_stack;
    mt_task_state_segment.ss0 = 2 << 3;
    mt_task_state_segment.iomap = sizeof(mt_task_state_segment);
}
