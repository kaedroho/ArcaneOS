#include "x86/sys.h"
#include "x86/gdt.h"
#include "x86/mt.h"
#include "string.h"

struct mt_process* mt_first_process = 0;
struct mt_process* mt_last_process = 0;
struct mt_process* mt_kernel_process = 0;
struct mt_thread* mt_current_thread = 0;
struct mt_thread* mt_schedule_first_thread = 0;
struct mt_thread* mt_schedule_last_thread = 0;
struct mt_thread* mt_sleeping_first_thread = 0;
struct mt_thread* mt_sleeping_last_thread = 0;

// These commands handle allocation of processes and threads,
// automatically updating the relevent linked lists and setting
// other variables to zero.
struct mt_process* mt_alloc_process() {
    struct mt_process* process = (struct mt_process*)mm_kernel_alloc(sizeof(struct mt_process));
    memset((unsigned char*)process, 0, sizeof(struct mt_process));

    if (mt_last_process)
        mt_last_process->next_process = process;
    else
        mt_first_process = process;

    process->prev_process = mt_last_process;    
    mt_last_process = process;

    return process;
}
void mt_free_process(struct mt_process* process) {
    if (process->prev_process)
        process->prev_process->next_process = process->next_process;
    else
        mt_first_process = process->next_process;

    if (process->next_process)
        process->next_process->prev_process = process->prev_process;
    else
        mt_last_process = process->prev_process;

    mm_kernel_free(process);
}
struct mt_thread* mt_alloc_thread(struct mt_process* process) {
    struct mt_thread* thread = (struct mt_thread*)mm_kernel_alloc(sizeof(struct mt_thread));
    memset((unsigned char*)thread, 0, sizeof(struct mt_thread));

    if (process->last_thread)
        process->last_thread->next_thread = thread;
    else
        process->first_thread = thread;

    thread->prev_thread = process->last_thread;
    process->last_thread = thread;

    return thread;
}
void mt_free_thread(struct mt_thread* thread) {
    if (thread->prev_thread)
        thread->prev_thread->next_thread = thread->next_thread;
    else
        thread->process->first_thread = thread->next_thread;

    if (thread->next_thread)
        thread->next_thread->prev_thread = thread->prev_thread;
    else
        thread->process->last_thread = thread->prev_thread;

    mm_kernel_free(thread);
}

void mt_init() {
    mt_kernel_process = mt_alloc_process();
    mt_current_thread = mt_alloc_thread(mt_kernel_process);
    
    mt_kernel_process->page_directory = pg_kernel_directory;
}
void* mt_alloc_stack() {
    // 4KB stack
    return mm_kernel_page_alloc(1);
}
void mt_free_stack(void* ptr) {
    // 4KB stack
    mm_kernel_page_free(ptr, 1);
}
struct mt_thread* mt_schedule_pop() {
    struct mt_thread* result = mt_schedule_first_thread;

    mt_schedule_first_thread = result->schedule_next;
    if (mt_schedule_first_thread)
        mt_schedule_first_thread->schedule_prev = 0;
    else
        mt_schedule_last_thread = 0;

    return result;
}
void mt_schedule_remove(struct mt_thread* thread) {
    if (thread->schedule_prev)
        thread->schedule_prev->schedule_next = thread->schedule_next;
    else
        mt_schedule_first_thread = thread->schedule_next;

    if (thread->schedule_next)
        thread->schedule_next->schedule_prev = thread->schedule_prev;
    else
        mt_schedule_last_thread = thread->schedule_prev;
}
void mt_schedule_insert_after(struct mt_thread* thread, struct mt_thread* place) {
    thread->schedule_prev = place;

    if (place) {
        thread->schedule_next = place->schedule_next;
        place->schedule_next = thread;
    } else {
        thread->schedule_next = mt_schedule_first_thread;
        mt_schedule_first_thread = thread;
    }

    if (thread->schedule_next)
        thread->schedule_next->schedule_prev = thread;
    else
        mt_schedule_last_thread = thread;
}
void mt_sleep_remove(struct mt_thread* thread) {
    // A thread's sleep_delay is relative to the delay of the previous thread,
    // so that each tick, only the first thread in the queue needs to be updated
    if (thread->schedule_next)
        thread->schedule_next->sleep_delay += thread->sleep_delay;

    if (thread->schedule_prev)
        thread->schedule_prev->schedule_next = thread->schedule_next;
    else
        mt_sleeping_first_thread = thread->schedule_next;

    if (thread->schedule_next)
        thread->schedule_next->schedule_prev = thread->schedule_prev;
    else
        mt_sleeping_last_thread = thread->schedule_prev;
}
void mt_sleep_insert_after(struct mt_thread* thread, struct mt_thread* place) {
    thread->schedule_prev = place;

    if (place) {
        thread->schedule_next = place->schedule_next;
        place->schedule_next = thread;
    } else {
        thread->schedule_next = mt_sleeping_first_thread;
        mt_sleeping_first_thread = thread;
    }

    if (thread->schedule_next)
        thread->schedule_next->schedule_prev = thread;
    else
        mt_sleeping_last_thread = thread;

    // A thread's sleep_delay is relative to the delay of the previous thread,
    // so that each tick, only the first thread in the queue needs to be updated
    if (thread->schedule_next)
        thread->schedule_next->sleep_delay -= thread->sleep_delay;
}
err_t mt_thread_sleep_impl(struct mt_thread* thread, unsigned delay) {
    if (thread->thread_state == MT_THREAD_STATE_SLEEPING)
        mt_sleep_remove(thread);
    else if (thread->thread_state == MT_THREAD_STATE_RUNNING)
        mt_schedule_remove(thread);

    thread->sleep_delay = delay;
    struct mt_thread* prev_place = 0;
    struct mt_thread* place;

    for (place = mt_sleeping_first_thread; 1; place = place->schedule_prev) {
        // If thread 'place' should be scheduled after this thread,
        // insert this thread before it in the queue.
        if (!place || (place->sleep_delay > thread->sleep_delay)) {
            mt_sleep_insert_after(thread, prev_place);
            break;
        }

        // Make sleep_delay relative to previous thread
        thread->sleep_delay -= place->sleep_delay;
        prev_place = place;
    }
    
    thread->thread_state = MT_THREAD_STATE_SLEEPING;

    return ERR_OK;
}
SYSCALL_DEFINE( mt_create_thread,
                syscall_state_kernel_cli, 0, syscall_datatype_ptr, syscall_datatype_ptr, syscall_datatype_ptr, syscall_datatype_ptr, syscall_datatype_s32
                )(struct mt_thread** out_thread, struct mt_process* process, void (*fptr)(void*), void* param, int usermode) {
    struct mt_thread* thread = mt_alloc_thread(process);
    void* stack = mt_alloc_stack();
    thread->regs = (struct regs*)((char*)stack + pg_page_size - sizeof(struct regs));
    thread->esp = (unsigned)&thread->regs->gs;

    if (usermode) {
        thread->regs->gs = GDT_USERMODE_PROT_MODE_DSEG;
        thread->regs->fs = GDT_USERMODE_PROT_MODE_DSEG;
        thread->regs->es = GDT_USERMODE_PROT_MODE_DSEG;
        thread->regs->ds = GDT_USERMODE_PROT_MODE_DSEG;
        thread->regs->cs = GDT_USERMODE_PROT_MODE_CSEG;

        // Allocate usermode stack
        thread->regs->useresp = (unsigned)((char*)mt_alloc_stack() + pg_page_size);
        thread->regs->ss = GDT_USERMODE_PROT_MODE_DSEG;

        // TODO: Push return value onto stack to automatically delete
        //       the thread when it returns.
        thread->regs->usermode_return_address = 0;
        thread->regs->usermode_param = (unsigned int)param;
    } else {
        thread->regs->gs = GDT_KERNEL_PROT_MODE_DSEG;
        thread->regs->fs = GDT_KERNEL_PROT_MODE_DSEG;
        thread->regs->es = GDT_KERNEL_PROT_MODE_DSEG;
        thread->regs->ds = GDT_KERNEL_PROT_MODE_DSEG;
        thread->regs->cs = GDT_KERNEL_PROT_MODE_CSEG;
        // TODO: Push return value onto stack to automatically delete
        //       the thread when it returns.
        thread->regs->kernel_return_address = 0;
        thread->regs->kernel_param = (unsigned int)param;
    }
    thread->regs->edi = 0;
    thread->regs->esi = 0;
    thread->regs->ebp = 0;
    thread->regs->esp = (unsigned int)&thread->regs->int_no;
    thread->regs->ebx = 0;
    thread->regs->edx = 0;
    thread->regs->ecx = 0;
    thread->regs->eax = 0;
    thread->regs->eip = (unsigned int)fptr;
    thread->regs->eflags = 0x200; // Enable interrupts for thread

    mt_schedule_insert_after(thread, mt_schedule_last_thread);

    if (out_thread)
        *out_thread = thread;

    return ERR_OK;
}
SYSCALL_DEFINE( mt_schedule,
                syscall_state_int_handler, 0, syscall_datatype_void
                )() {
    struct regs* regs = irq_regs;

    // If there is at least one thread waiting in the queue
    if (mt_schedule_first_thread) {
        // Save current thread's registers
        mt_current_thread->regs = regs;
        mt_current_thread->esp = regs->new_esp;
        // Reschedule current thread
        mt_schedule_insert_after(mt_current_thread, mt_schedule_last_thread);

        // Find next thread to make active
        mt_current_thread = mt_schedule_pop();
        // Switch stack to that of the next thread
        regs->new_esp = mt_current_thread->esp;
    }
    return ERR_OK;
}
SYSCALL_DEFINE( mt_tick,
                syscall_state_kernel_cli, 0, syscall_datatype_u32
                )(unsigned ticks) {
    struct mt_thread* thread;
    while ((thread = mt_sleeping_first_thread)) {
        if (thread->sleep_delay <= ticks) {
            ticks -= thread->sleep_delay;
            mt_sleep_remove(thread);
            mt_schedule_insert_after(thread, mt_schedule_last_thread);
            thread->thread_state = MT_THREAD_STATE_RUNNING;
        } else {
            thread->sleep_delay -= ticks;
            break;
        }
    }
    
    return ERR_OK;
}

SYSCALL_DEFINE( mt_sleep,
                syscall_state_int_handler, 0, syscall_datatype_u32
                )(unsigned delay) {
    struct mt_thread* thread = mt_current_thread;
    syscall(&mt_schedule);
    mt_thread_sleep_impl(thread, delay);
    
    return ERR_OK;
}

SYSCALL_DEFINE( mt_thread_sleep,
                syscall_state_none, 0, syscall_datatype_ptr, syscall_datatype_u32
                )(struct mt_thread* thread, unsigned delay) {
    err_t result;

    if (thread != mt_current_thread) {
        unsigned handle = irq_lock();
        result = mt_thread_sleep_impl(thread, delay);
        irq_unlock(handle);
    } else
        result = syscall(&mt_sleep, delay);

    return result;
}
