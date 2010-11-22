#include "x86/sys.h"
#include "x86/syscall.h"

unsigned int syscall_datasize[syscall_datatype_count] = {
    0,
    sizeof(unsigned char),
    sizeof(unsigned char),
    sizeof(signed char),
    sizeof(unsigned short),
    sizeof(signed short),
    sizeof(unsigned int),
    sizeof(signed int),
    sizeof(unsigned long long),
    sizeof(signed long long),
    sizeof(float),
    sizeof(double),
    sizeof(void*),
    sizeof(char*),
    sizeof(void*)
};

void syscall_handler(struct regs* r) {
    // Allow registers to be accessible anywhere
    irq_regs = r;

    // Perform call
    r->eax = syscall_call((void*)r->eax, (void*)r->ecx, r->edx);

    // Registers no longer valid
    irq_regs = 0;
}

extern void syscall_stub();
void syscall_init() {
    idt_set_gate(0x80, (unsigned)&syscall_stub, 0x08, 0x8E);
}

void syscall_function_init(struct syscall_function* func) {
    if (func->is_initialised)
        return;

    unsigned i;
    func->parameter_count = 0;
    func->parameter_bytes = 0;
    for (i = 0; (i < 32) && (func->parameters[i] != syscall_datatype_void); i++) {
        func->parameter_count++;
        func->parameter_bytes += syscall_datasize[func->parameters[i]];
    }

    func->is_initialised = 1;
}

err_t syscall(struct syscall_function* func, ...) {
    if (!func)
        return ERR(1, err_facility_syscall, syscall_err_invalidcall);

    err_t result;

    syscall_function_init(func);
    
    void* params = (&func + 1);
    void* fptr = func->fptr;

    unsigned handle;

    // TODO: Handle mutex

    switch (func->required_state) {
    case syscall_state_none:
        // Call function directly
        result = syscall_call(fptr, params, func->parameter_bytes);
        break;
    case syscall_state_kernel_sti:
        // Call function if interrupts are enabled
        if (irq_query())
            result = syscall_call(fptr, params, func->parameter_bytes);
        else
            return ERR(1, err_facility_syscall, syscall_err_invalidcall);
        break;
    case syscall_state_kernel_cli:
        // Disable interrupts before calling function
        handle = irq_lock();
        result = syscall_call(fptr, params, func->parameter_bytes);
        irq_unlock(handle);
        break;
    case syscall_state_int_handler:
        // Call function directly if in interrupt handler, otherwise raise interrupt if possible
        if (irq_regs)
            result = syscall_call(fptr, params, func->parameter_bytes);
        else if (irq_query()) {
            result = syscall_interrupt(fptr, params, func->parameter_bytes);
        } else
            return ERR(1, err_facility_syscall, syscall_err_invalidcall);
    default:
        return ERR(1, err_facility_syscall, syscall_err_invalidcall);
    }

    return result;
}
