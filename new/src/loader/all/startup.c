#include <boot.h>
#include <console.h>
#include <kb.h>
#include <boot.h>
#include <screen.h>

#define ARCANE_VERSION_STRING "0.01"

void kload();
void* pg_physical_page_alloc();
void pg_physical_page_free(void* ptr);

int startup()
{
//Get multiboot info
    struct multiboot_information* mboot=get_multiboot_info();

//Print boot message
    console_puts_protected("BOOT: ");
    console_puts_protected("ArcaneOS version ");
    console_puts_protected(ARCANE_VERSION_STRING);
    console_puts_protected(". Booted by ");
    console_puts_protected(mboot->boot_loader_name);
    console_puts_protected(".\n");

//Load the kernel
    kload();
    kb_init();

    console_putc('\n');

__asm__ __volatile__ ("sti");

    return 0;
}