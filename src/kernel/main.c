#include "sys.h"
#include "cli.h"
#include "ui/ui.h"
#include "boot.h"

void putint(unsigned int i, unsigned int base)
{
    char buffer[32] = {0};
    int j;
    for (j = 31; i || (j == 31); i /= base)
    {
         buffer[--j] = (i%base) + '0';
         if (buffer[j] > '9')
            buffer[j] += 'A'-'9';
    }
    cli_puts(buffer+j);
}

int main()
{
//Put loading message
    cli_puts("ArcaneOS Loading...\n");

//Setup kernel
    gdt_init();
    idt_init();
    isr_init();
    irq_init();
    timer_init();
    kb_init();
    ui_init();

//Start interrupts
    __asm__ __volatile__ ("sti");

// Debug info (TEMP)
    struct multiboot_information* multiboot_info = get_multiboot_info();

    cli_puts("\nFlags: %"); putint(multiboot_info->flags,2);
    if (multiboot_info->flags & 1)
    {
        cli_puts("\n\nMEMORY:");
        cli_puts("\nMemory lower: "); putint(multiboot_info->mem_lower,10);
        cli_puts("\nMemory upper: "); putint(multiboot_info->mem_upper,10);
    }
    if (multiboot_info->flags & 2)
    {
        cli_puts("\n\nBOOT DEVICE:");
        cli_puts("\nDrive: "); putint(multiboot_info->boot_device.drive,10);
        cli_puts("\nPart 1: "); putint(multiboot_info->boot_device.part1,10);
        cli_puts("\nPart 2: "); putint(multiboot_info->boot_device.part2,10);
        cli_puts("\nPart 3: "); putint(multiboot_info->boot_device.part3,10);
    }
    if (multiboot_info->flags & 4)
    {
        cli_puts("\n\nCOMMAND LINE:");
        cli_puts("\nCommand line: "); cli_puts(multiboot_info->cmdline);
    }
    if (multiboot_info->flags & 8)
    {
        cli_puts("\n\nMODULES:");
        cli_puts("\nModule count: "); putint(multiboot_info->mods_count,10);
        cli_puts("\nModules address: 0x"); putint((unsigned int)multiboot_info->mods_addr,16);
    }
    if (multiboot_info->flags & 16)
    {
        cli_puts("\n\nSYMBOL TABLE:");
        cli_puts("\nTable size: "); putint(multiboot_info->symboltable.tabsize,10);
        cli_puts("\nString size: "); putint(multiboot_info->symboltable.strsize,10);
        cli_puts("\nAddress: 0x"); putint(multiboot_info->symboltable.addr,16);
    }
    if (multiboot_info->flags & 32)
    {
        cli_puts("\n\nSECTION TABLE:");
        cli_puts("\nCount: "); putint(multiboot_info->sectiontable.num,10);
        cli_puts("\nSize: "); putint(multiboot_info->sectiontable.size,10);
        cli_puts("\nAddress: 0x"); putint(multiboot_info->sectiontable.addr,16);
        cli_puts("\nSHNDX: "); putint(multiboot_info->sectiontable.shndx,10);
    }
    if (multiboot_info->flags & 64)
    {
        cli_puts("\n\nMEMORY MAP:");
        cli_puts("\nMMap address: 0x"); putint(multiboot_info->mmap_addr,16);
        cli_puts("\nMMap length: "); putint(multiboot_info->mmap_length,10);
    }
    if (multiboot_info->flags & 128)
    {
        cli_puts("\n\nDRIVES:");
        cli_puts("\nDrives length: "); putint(multiboot_info->drives_length,10);
        cli_puts("\nDrives address: 0x"); putint((unsigned int)multiboot_info->drives_addr,16);
    }
    if (multiboot_info->flags & 256)
    {
        cli_puts("\n\nCONFIG TABLE:");
        cli_puts("\nConfig table: "); putint(multiboot_info->config_table,10);
    }
    if (multiboot_info->flags & 512)
    {
        cli_puts("\n\nBOOT LOADER:");
        cli_puts("\nName: "); cli_puts(multiboot_info->boot_loader_name);
    }

    //for (;;); // UNCOMMENT THIS TO SEE DEBUG INFO

//Start ui
    ui_start();

//Endless loop
    for(;;);
}
