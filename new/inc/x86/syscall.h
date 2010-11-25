/* 
 * File:   syscall.h
 * Author: Diggory
 *
 * Created on 20 November 2010, 19:20
 */

#ifndef SYSCALL_H
#define	SYSCALL_H

/* +----------------------------+ */
/* | SYSCALL ERR SPECIFICATION  | */
/* +--------+-------------------+ */
/* | BITS   | MEANING           | */
/* +--------+-------------------+ */
/* |  0-15  | ERROR CODE        | */
/* | 16-23  | FACILITY          | */
/* | 24-30  | RESERVED          | */
/* |    31  | FAILURE           | */
/* +--------+-------------------+ */

typedef unsigned int err_t;

#define ERR_SUCCEEDED(r)    ((r)>>31 == 0)
#define ERR_FAILED(r)       ((r)>>31 != 0)
#define ERR_CODE(r)     ((r) & 0xFFFF)
#define ERR_FACILITY(r) (((r)>>16)&0xFF)
#define ERR(failed, facility, code) ( \
    (((err_t)(failed)&1)<<31)| \
    (((err_t)(facility)&0xFF)<<16)| \
    ((err_t)(code)&0xFFFF) \
    )

#define ERR_OK ERR(0, err_facility_none, 0)

enum err_facility {
    err_facility_none = 0,
    err_facility_syscall,
    err_facility_mt,
    err_facility_pg,
    err_facility_floppy,
    err_facility_sync
};

enum syscall_err_code {
    syscall_err_none = 0,
    syscall_err_invalidcall
};

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
    syscall_datatype_ptr,
    syscall_datatype_string,
    syscall_datatype_stream,
    syscall_datatype_count
};

extern unsigned int syscall_datasize[syscall_datatype_count];

struct syscall_function {
    enum syscall_state required_state; // Required state to call function
    void* fptr; // Function pointer
    struct sync_mutex* mutex; // Mutex (ownership required before syscall)
    enum syscall_datatype parameters[32];
    int is_initialised;
    unsigned parameter_count;
    unsigned parameter_bytes;
};

void syscall_init();
err_t syscall_call(void* fptr, void* params, unsigned param_bytes);
err_t syscall_interrupt(void* fptr, void* params, unsigned param_bytes);
err_t syscall(struct syscall_function* func, ...);

#define SYSCALL_DECLARE(name, ...) \
    extern struct syscall_function name; \
    err_t _##name

#define SYSCALL_DEFINE(name, required_state, mutex, ...) \
    struct syscall_function name = { \
        required_state, \
        &_##name, \
        mutex, \
        {__VA_ARGS__, syscall_datatype_void}, \
        0 \
    }; \
    err_t _##name

#endif	/* SYSCALL_H */

