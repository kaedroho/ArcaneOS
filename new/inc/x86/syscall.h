/* 
 * File:   syscall.h
 * Author: Diggory
 *
 * Created on 20 November 2010, 19:20
 */

#ifndef SYSCALL_H
#define	SYSCALL_H

enum syscall_state {
    syscall_state_none = 0,     // Simply call function directly
    syscall_state_kernel_sti,   // Kernel mode with interrupts enabled
    syscall_state_kernel_cli,   // Kernel mode with interrupts disabled (minimize use)
    syscall_state_int_handler,  // Called from interrupt handler (minimize use)
    syscall_state_count
};

enum syscall_datatype {
    syscall_datatype_void = 0,
    syscall_datatype_bool,
    syscall_datatype_u8,
    syscall_datatype_s8,
    syscall_datatype_u16,
    syscall_datatype_s16,
    syscall_datatype_u32,
    syscall_datatype_s32,
    syscall_datatype_u64,
    syscall_datatype_s64,
    syscall_datatype_f32,
    syscall_datatype_f64,
    syscall_datatype_string,
    syscall_datatype_stream,
    syscall_datatype_count
};

extern unsigned int syscall_datasize[syscall_datatype_count];

struct syscall_function {
    enum syscall_state required_state; // Required state to call function
    void* fptr; // Function pointer
    void* mutex; // Mutex (ownership required before syscall)
    enum syscall_datatype return_type;
    enum syscall_datatype parameters[32];
    int is_initialised;
    unsigned parameter_count;
    unsigned parameter_bytes;
    unsigned return_bytes;
};

void syscall_init();
void syscall_call(void* fptr, void* params, unsigned param_bytes);
void syscall_interrupt(void* fptr, void* params, unsigned param_bytes);
int syscall(struct syscall_function* func, ...);

#define SYSCALL_DECLARE(type, name, ...) \
    extern struct syscall_function name; \
    type _##name

#define SYSCALL_DEFINE(type, name, required_state, mutex, return_type, ...) \
    struct syscall_function name = { \
        required_state, \
        &_##name, \
        mutex, \
        return_type, \
        {__VA_ARGS__, syscall_datatype_void}, \
        0 \
    }; \
    type _##name

#endif	/* SYSCALL_H */

