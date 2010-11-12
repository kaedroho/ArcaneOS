// This file handles code which must run in the lower memory of the computer

#include <console.h>
#include <string.h>

// Addresses of these variable are set by the linker
// DO NOT use these variable, only take the address of them!
extern unsigned char low_code, low_code_end, low_phys;

void lm_init() {
    // Copy code to low memory
    unsigned char* code_dest = &low_phys, *code_src = &low_code;
    unsigned int code_size = &low_code_end - &low_code;

    console_puts_protected("LOWMEM:");
    memcpy(code_dest, code_src, code_size);
    console_puts_protected(" Copied ");
    console_putu32_protected(code_size, 10);
    console_puts_protected(" bytes from 0x");
    console_putu32_protected((unsigned)code_src, 16);
    console_puts_protected(" to 0x");
    console_putu32_protected((unsigned)code_dest, 16);
    console_puts_protected("\n");
}
