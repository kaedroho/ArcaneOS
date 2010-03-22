#include "paging.h"
#include "mm.h"
#include "libs/memory.h"
#include "boot.h"
#include "cli.h"
#include "sys.h"

// A safe stack to use when paging is disabled
struct pg_directory* pg_kernel_directory = 0;

void pg_init()
{
    pg_kernel_directory = pg_alloc_directory();

    // Map reserved memory directly from virtual to physical
    unsigned int ptr;
    for (ptr = 0; ptr < mm_reserved_end; ptr += 4096)
        pg_map_page(pg_kernel_directory,ptr,ptr,1,1);

    struct multiboot_information* boot_info = get_multiboot_info();

    unsigned int page_index;

    if (boot_info->flags & 64)
    {
        struct multiboot_mmap* mmap = boot_info->mmap_addr;
        while(mmap < boot_info->mmap_addr + boot_info->mmap_length)
        {
            if ((mmap->type != 1) && (mmap->type != 5) && (mmap->base_addr_high == 0) && (mmap->length_high == 0))
            {
                unsigned int page_start = (mmap->base_addr_low)/mm_page_size;
                unsigned int page_end = (mmap->base_addr_low + mmap->length_low + mm_page_size - 1)/mm_page_size;

                if (page_start < mm_reserved_end/mm_page_size)
                    page_start = mm_reserved_end/mm_page_size;

                //cli_puts("\nReserving pages "); cli_putu32(page_start,10);
                //cli_puts(" to "); cli_putu32(page_end,10);

                for (page_index = page_start; page_index < page_end; page_index++)
                    pg_map_page(pg_kernel_directory,page_index*mm_page_size,page_index*mm_page_size,0,1);
            }

            mmap = (struct multiboot_mmap*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
        }
    }

    // Enable paging
    pg_set_directory(pg_kernel_directory);

    __asm__ __volatile__ ("mov %cr0, %eax");
    __asm__ __volatile__ ("or $0x80000000, %eax");
    __asm__ __volatile__ ("mov %eax, %cr0");
}
/*
void pg_set_enabled(int enabled)
{
    if (enabled)
    {
        __asm__ __volatile__ ("mov %cr0, %eax");
        __asm__ __volatile__ ("or $0x80000000, %eax");
        __asm__ __volatile__ ("mov %eax, %cr0");
    }
    else
    {
        __asm__ __volatile__ ("mov %cr0, %eax");
        __asm__ __volatile__ ("and $0x7FFFFFFF, %eax");
        __asm__ __volatile__ ("mov %eax, %cr0");
    }
}
int pg_get_enabled()
{
    unsigned int cr0 = 0;
    __asm__ __volatile__ ("mov %%cr0, %0" : "=a" (cr0));
    return (cr0 & 0x80000000) != 0;
}
int pg_lock()
{
    int result = pg_get_enabled();
    pg_set_enabled(0);
    return result;
}
void pg_unlock(int handle)
{
    pg_set_enabled(handle);
}
*/
void pg_set_directory(struct pg_directory* directory)
{
    __asm__ __volatile__ ("mov %0, %%cr3" : : "a" (directory));
}
struct pg_directory* pg_get_directory()
{
    struct pg_directory* directory = 0;
    __asm__ __volatile__ ("mov %%cr3, %0" : "=a" (directory));
    return directory;
}
struct pg_directory* pg_alloc_directory()
{
    struct pg_directory* dir = (struct pg_directory*)mm_physical_page_alloc();
    memset((unsigned char*)dir,0,4096);
    return dir;
}
void pg_free_directory(struct pg_directory* directory)
{
    mm_physical_page_free(directory);
}
struct pg_table* pg_alloc_table()
{
    struct pg_table* tbl = (struct pg_table*)mm_physical_page_alloc();
    memset((unsigned char*)tbl,0,4096);
    return tbl;
}
void pg_free_table(struct pg_table* table)
{
    mm_physical_page_free(table);
}

unsigned int _pg_physical_address;
unsigned int _pg_writable;
unsigned int _pg_present;
struct pg_directory* _pg_directory;
unsigned int _pg_count;
unsigned int _pg_result;
unsigned int _pg_virtual_address;

void _pg_map_page(struct pg_directory* directory, unsigned int physical_address, unsigned int virtual_address, unsigned int writable, unsigned int present)
{
    unsigned int dir_index = (virtual_address >> 22) & 0x3FF;
    unsigned int pag_index = (virtual_address >> 12) & 0x3FF;

    unsigned int pgval = (physical_address & 0xFFFFF000) | (1 << 3) | (1 << 2) | ((writable & 1) << 1) | (present & 1);

    struct pg_table* table = (struct pg_table*)(directory->tables[dir_index] & 0xFFFFF000);

    if (!table)
        // Allocate new table
        directory->tables[dir_index] = 7+(unsigned int)(table = pg_alloc_table());

    // Map page
    table->pages[pag_index] = pgval;
}
void pg_map_page(struct pg_directory* directory, unsigned int physical_address, unsigned int virtual_address, unsigned int writable, unsigned int present)
{
    unsigned int handle_irq = irq_lock();

    _pg_directory = directory;
    _pg_physical_address = physical_address;
    _pg_virtual_address = virtual_address;
    _pg_writable = writable;
    _pg_present = present;

    __asm__ __volatile__ ("mov %esp, %eax");
    __asm__ __volatile__ ("mov _paging_stack, %esp");
    __asm__ __volatile__ ("push %eax");

    __asm__ __volatile__ ("mov %cr0, %eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("and $0x7FFFFFFF, %eax");
    __asm__ __volatile__ ("mov %eax, %cr0");

    __asm__ __volatile__ ("mov  (_pg_present),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("mov  (_pg_writable),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("mov  (_pg_virtual_address),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("mov  (_pg_physical_address),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("mov  (_pg_directory),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("call _pg_map_page");
    __asm__ __volatile__ ("add  $20,%esp");

    __asm__ __volatile__ ("pop %eax");
    __asm__ __volatile__ ("mov %eax, %cr0");

    __asm__ __volatile__ ("pop %eax");
    __asm__ __volatile__ ("mov %eax, %esp");

    irq_unlock(handle_irq);
}
unsigned int _pg_find_virtual_pages(struct pg_directory* directory, unsigned int count)
{
    unsigned int dir_index;
    unsigned int pag_index;
    unsigned int num_continuous = 0;
    unsigned int dir_item;

    // Find free pages in an existing table
    for (dir_index = 1; dir_index < 0x3FF; dir_index++)
    {
        dir_item = directory->tables[dir_index];
        if (dir_item & 1)
        {
            struct pg_table* table = (struct pg_table*)(dir_item & 0xFFFFF000);
            for (pag_index = 0; pag_index < 0x3FF; pag_index++)
            {
                if (!(table->pages[pag_index] & 1))
                {
                    num_continuous++;
                    if (num_continuous >= count)
                        return (dir_index << 22) | ((pag_index+1-count) << 12);
                }
                else
                    num_continuous = 0;
            }
        }
        else
            num_continuous = 0;
    }

    // Create a new table
    unsigned int dir_count = (count+0x3FF)/0x400;
    num_continuous = 0;
    for (dir_index = 1; dir_index < 0x3FF; dir_index++)
    {
        dir_item = directory->tables[dir_index];
        if (!(dir_item & 1))
        {
            num_continuous++;
            if (num_continuous >= dir_count)
                return ((dir_index+1-dir_count) << 22);
        }
        else
            num_continuous = 0;
    }

    return 0;
}
unsigned int pg_find_virtual_pages(struct pg_directory* directory, unsigned int count)
{
    unsigned int handle_irq = irq_lock();

    _pg_directory = directory;
    _pg_count = count;


    __asm__ __volatile__ ("mov %esp, %eax");
    __asm__ __volatile__ ("mov _paging_stack, %esp");
    __asm__ __volatile__ ("push %eax");

    __asm__ __volatile__ ("mov %cr0, %eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("and $0x7FFFFFFF, %eax");
    __asm__ __volatile__ ("mov %eax, %cr0");

    __asm__ __volatile__ ("mov  (_pg_count),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("mov  (_pg_directory),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("call _pg_find_virtual_pages");
    __asm__ __volatile__ ("add  $8,%esp");
    __asm__ __volatile__ ("mov  %eax,(_pg_result)");

    //_pg_result = _pg_find_virtual_pages(_pg_directory, _pg_count);


    __asm__ __volatile__ ("pop %eax");
    __asm__ __volatile__ ("mov %eax, %cr0");

    __asm__ __volatile__ ("pop %eax");
    __asm__ __volatile__ ("mov %eax, %esp");


    irq_unlock(handle_irq);

    return _pg_result;
}
unsigned int _pg_virtual_to_physical(struct pg_directory* directory, unsigned int virtual_address)
{
    unsigned int dir_index = (virtual_address >> 22) & 0x3FF;
    unsigned int pag_index = (virtual_address >> 12) & 0x3FF;
    unsigned int offset = virtual_address & 0xFFF;

    unsigned int dir_entry = directory->tables[dir_index];
    if ((dir_entry & 1) == 0)
        return 0;

    struct pg_table* table = (struct pg_table*)(dir_entry & 0xFFFFF000);

    unsigned int pag_entry = table->pages[pag_index];
    if ((pag_entry & 1) == 0)
        return 0;

    unsigned int physical_address = (pag_entry & 0xFFFFF000) + offset;

    return physical_address;
}

unsigned int pg_virtual_to_physical(struct pg_directory* directory, unsigned int virtual_address)
{
    unsigned int handle_irq = irq_lock();

    _pg_directory = directory;
    _pg_virtual_address = virtual_address;

    __asm__ __volatile__ ("mov %esp, %eax");
    __asm__ __volatile__ ("mov _paging_stack, %esp");
    __asm__ __volatile__ ("push %eax");

    __asm__ __volatile__ ("mov %cr0, %eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("and $0x7FFFFFFF, %eax");
    __asm__ __volatile__ ("mov %eax, %cr0");

    __asm__ __volatile__ ("mov  (_pg_virtual_address),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("mov  (_pg_directory),%eax");
    __asm__ __volatile__ ("push %eax");
    __asm__ __volatile__ ("call _pg_virtual_to_physical");
    __asm__ __volatile__ ("add  $8,%esp");
    __asm__ __volatile__ ("mov  %eax,(_pg_result)");

    //_pg_result = _pg_virtual_to_physical(_pg_directory,_pg_virtual_address);

    __asm__ __volatile__ ("pop %eax");
    __asm__ __volatile__ ("mov %eax, %cr0");

    __asm__ __volatile__ ("pop %eax");
    __asm__ __volatile__ ("mov %eax, %esp");

    irq_unlock(handle_irq);

    return _pg_result;
}

