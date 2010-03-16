#ifndef MT_H_INCLUDED
#define MT_H_INCLUDED

#include "sys.h"

struct mt_tss {
    unsigned short   link;
    unsigned short   link_h;

    unsigned long   esp0;
    unsigned short   ss0;
    unsigned short   ss0_h;

    unsigned long   esp1;
    unsigned short   ss1;
    unsigned short   ss1_h;

    unsigned long   esp2;
    unsigned short   ss2;
    unsigned short   ss2_h;

    unsigned long   cr3;
    unsigned long   eip;
    unsigned long   eflags;

    unsigned long   eax;
    unsigned long   ecx;
    unsigned long   edx;
    unsigned long    ebx;

    unsigned long   esp;
    unsigned long   ebp;

    unsigned long   esi;
    unsigned long   edi;

    unsigned short   es;
    unsigned short   es_h;

    unsigned short   cs;
    unsigned short   cs_h;

    unsigned short   ss;
    unsigned short   ss_h;

    unsigned short   ds;
    unsigned short   ds_h;

    unsigned short   fs;
    unsigned short   fs_h;

    unsigned short   gs;
    unsigned short   gs_h;

    unsigned short   ldt;
    unsigned short   ldt_h;

    unsigned short   trap;
    unsigned short   iomap;

};

struct mt_process
{
    struct mt_process* prev_process;
    struct mt_process* next_process;
    struct mt_thread* first_thread;
    struct mt_thread* last_thread;
    struct pg_directory* directory;
};

struct mt_thread
{
    struct mt_process* process;
    struct mt_thread* prev_thread;
    struct mt_thread* next_thread;
    unsigned int stack_base;
    unsigned int stack_ptr;
    unsigned int sleep_time; // ms
};

extern struct mt_tss mt_task_state_segment;
extern unsigned int mt_syscall_stack;
extern unsigned int mt_thread_stack_ptr;
extern struct mt_process* mt_first_process;
extern struct mt_process* mt_last_process;
extern struct mt_process* mt_kernel_process;

extern unsigned int mt_context_switch_delay;

extern void mt_init();

extern struct mt_thread* mt_create_thread(struct mt_process* process, void* eip, int stack_pages);
extern struct mt_process* mt_create_process(void* eip, struct pg_directory* directory, int stack_pages);

extern void mt_switch(struct regs* state);
extern void mt_sleep(struct mt_thread* thread, unsigned int duration);

#endif // PROCESS_H_INCLUDED
