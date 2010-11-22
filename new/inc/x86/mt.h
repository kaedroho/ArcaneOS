/* 
 * File:   mt.h
 * Author: Diggory
 *
 * Created on 19 November 2010, 18:27
 */

#ifndef MT_H
#define	MT_H

#include "x86/syscall.h"

struct mt_thread {
    struct mt_process* process;
    struct mt_thread* prev_thread;
    struct mt_thread* next_thread;
    unsigned int esp;
    struct regs* regs;
    int priority;
    struct mt_thread* schedule_prev;
    struct mt_thread* schedule_next;
    int thread_state;
    unsigned sleep_delay;
};

struct mt_process {
    struct mt_process* prev_process;
    struct mt_process* next_process;
    struct mt_thread* first_thread;
    struct mt_thread* last_thread;
    struct pg_page_directory* page_directory;
};

extern struct mt_process* mt_first_process;
extern struct mt_process* mt_last_process;
extern struct mt_process* mt_kernel_process;
extern struct mt_thread* mt_current_thread;
extern struct mt_thread* mt_schedule_first_thread;
extern struct mt_thread* mt_schedule_last_thread;
extern struct mt_thread* mt_sleeping_first_thread;
extern struct mt_thread* mt_sleeping_last_thread;

#define MT_THREAD_STATE_RUNNING 0
#define MT_THREAD_STATE_PAUSED 1
#define MT_THREAD_STATE_SLEEPING 2

struct mt_process* mt_alloc_process();
void mt_free_process(struct mt_process* process);
struct mt_thread* mt_alloc_thread(struct mt_process* process);
void mt_free_thread(struct mt_thread* thread);
void mt_init();
void* mt_alloc_stack();
void mt_free_stack(void* ptr);
struct mt_thread* mt_schedule_pop();
void mt_schedule_remove(struct mt_thread* thread);
void mt_schedule_insert_after(struct mt_thread* thread, struct mt_thread* place);
void mt_sleep_remove(struct mt_thread* thread);
void mt_sleep_insert_after(struct mt_thread* thread, struct mt_thread* place);
err_t mt_thread_sleep_impl(struct mt_thread* thread, unsigned delay);

SYSCALL_DECLARE(mt_create_thread)(struct mt_thread** out_thread, struct mt_process* process, void (*fptr)(void*), void* param, int usermode);
SYSCALL_DECLARE(mt_schedule)();
SYSCALL_DECLARE(mt_tick)(unsigned ticks);
SYSCALL_DECLARE(mt_sleep)(unsigned delay);
SYSCALL_DECLARE(mt_thread_sleep)(struct mt_thread* thread, unsigned delay);

#endif	/* MT_H */

