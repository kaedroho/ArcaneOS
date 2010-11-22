#include <x86/boot.h>
#include <x86/ibmpc.h>
#include <x86/mt.h>
#include <x86/syscall.h>
#include <console.h>
#include <clock.h>

#define ARCANE_VERSION_STRING "0.01"

void kload();
void* pg_physical_page_alloc();
void pg_physical_page_free(void* ptr);

void test(void* param) {
    while (1) {
        console_puts_protected("Test\n");
        syscall(&mt_sleep, 500);
    }
}

int startup()
{
//Get multiboot info
    struct multiboot_information* mboot=get_multiboot_info();

//Print boot message
    console_puts_protected("BOOT: ");
    console_puts_protected("ArcaneOS version ");
    console_puts_protected(ARCANE_VERSION_STRING);
    if (mboot) {
        console_puts_protected(". Booted by ");
        
        // Not all multiboot boot loaders tell us their name
        if (MB_GET_FLAG(mboot->flags, 9))
            console_puts_protected(mboot->boot_loader_name);
        else
            console_puts_protected("unnamed multiboot boot loader");
    }
    console_puts_protected(".\n");

//Load the kernel
    kload();
    ibmpc_init();
    clock_init();

    struct mt_thread* thread = 0;
    syscall(&mt_create_thread, &thread, mt_kernel_process, &test, (void*)0x1234, 0);
    
    console_puts_protected("KERNEL: Interrupts started!\n");
    console_putc('\n');
__asm__ __volatile__ ("sti");


    return 0;
}