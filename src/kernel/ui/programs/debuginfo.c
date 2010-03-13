#include "boot.h"

#include "cli.h"
#include "ui/ui.h"
#include "ui/programs.h"

void program_debuginfo_start()
{
//Print message
    cli_puts("Welcome to the ArcaneOS Debug info program!\n");
    cli_puts("Press Esc to exit this program.\n");

//Print debug info
    struct multiboot_information* multiboot_info = get_multiboot_info();

    cli_puts("\nFlags: %"); cli_putu32(multiboot_info->flags,2);
    if (multiboot_info->flags & 1)
    {
        cli_puts("\n\nMEMORY:");
        cli_puts("\nMemory lower: "); cli_putu32(multiboot_info->mem_lower,10);
        cli_puts("\nMemory upper: "); cli_putu32(multiboot_info->mem_upper,10);
    }
    if (multiboot_info->flags & 2)
    {
        cli_puts("\n\nBOOT DEVICE:");
        cli_puts("\nDrive: "); cli_putu32(multiboot_info->boot_device.drive,10);
        cli_puts("\nPart 1: "); cli_putu32(multiboot_info->boot_device.part1,10);
        cli_puts("\nPart 2: "); cli_putu32(multiboot_info->boot_device.part2,10);
        cli_puts("\nPart 3: "); cli_putu32(multiboot_info->boot_device.part3,10);
    }
    if (multiboot_info->flags & 4)
    {
        cli_puts("\n\nCOMMAND LINE:");
        cli_puts("\nCommand line: "); cli_puts(multiboot_info->cmdline);
    }
    if (multiboot_info->flags & 8)
    {
        cli_puts("\n\nMODULES:");
        cli_puts("\nModule count: "); cli_putu32(multiboot_info->mods_count,10);
        cli_puts("\nModules address: 0x"); cli_putu32((unsigned int)multiboot_info->mods_addr,16);
    }
    if (multiboot_info->flags & 16)
    {
        cli_puts("\n\nSYMBOL TABLE:");
        cli_puts("\nTable size: "); cli_putu32(multiboot_info->symboltable.tabsize,10);
        cli_puts("\nString size: "); cli_putu32(multiboot_info->symboltable.strsize,10);
        cli_puts("\nAddress: 0x"); cli_putu32(multiboot_info->symboltable.addr,16);
    }
    if (multiboot_info->flags & 32)
    {
        cli_puts("\n\nSECTION TABLE:");
        cli_puts("\nCount: "); cli_putu32(multiboot_info->sectiontable.num,10);
        cli_puts("\nSize: "); cli_putu32(multiboot_info->sectiontable.size,10);
        cli_puts("\nAddress: 0x"); cli_putu32(multiboot_info->sectiontable.addr,16);
        cli_puts("\nSHNDX: "); cli_putu32(multiboot_info->sectiontable.shndx,10);
    }
    if (multiboot_info->flags & 64)
    {
        cli_puts("\n\nMEMORY MAP:");
        cli_puts("\nMMap address: 0x"); cli_putu32((unsigned int)multiboot_info->mmap_addr,16);
        cli_puts("\nMMap length: "); cli_putu32(multiboot_info->mmap_length,10);
    }
    if (multiboot_info->flags & 128)
    {
        cli_puts("\n\nDRIVES:");
        cli_puts("\nDrives length: "); cli_putu32(multiboot_info->drives_length,10);
        cli_puts("\nDrives address: 0x"); cli_putu32((unsigned int)multiboot_info->drives_addr,16);
    }
    if (multiboot_info->flags & 256)
    {
        cli_puts("\n\nCONFIG TABLE:");
        cli_puts("\nConfig table: "); cli_putu32(multiboot_info->config_table,10);
    }
    if (multiboot_info->flags & 512)
    {
        cli_puts("\n\nBOOT LOADER:");
        cli_puts("\nName: "); cli_puts(multiboot_info->boot_loader_name);
    }
}

void program_debuginfo_kbhandler(unsigned char scancode)
{
//Esc quits an application
    if(scancode==0x01)
        ui_tab_programs_endprogram();
}
