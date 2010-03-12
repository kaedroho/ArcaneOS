#include "paging.h"
#include "mm.h"
#include "libs/memory.h"

struct pg_directory* pg_kernel_directory = 0;

void pg_init()
{
    pg_kernel_directory = pg_alloc_directory();

    // Map reserved memory directly from virtual to physical
    unsigned int ptr;
    for (ptr = 0; ptr < mm_reserved_end; ptr += 4096)
        pg_map_page(pg_kernel_directory,ptr,ptr,1,1);

    // Enable paging
    pg_set_directory(pg_kernel_directory);
    pg_set_enabled(1);
}
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
void pg_map_page(struct pg_directory* directory, unsigned int physical_address, unsigned int virtual_address, unsigned int writable, unsigned int present)
{
    int paging_enabled = pg_get_enabled();

    if (paging_enabled)
        pg_set_enabled(0);

    unsigned int dir_index = (virtual_address >> 22) & 0x3FF;
    unsigned int pag_index = (virtual_address >> 12) & 0x3FF;

    struct pg_table* table = (struct pg_table*)(directory->tables[dir_index] & 0xFFFFF000);

    if (!table)
        // Allocate new table
        directory->tables[dir_index] = 7+(unsigned int)(table = pg_alloc_table());

    // Map page
    table->pages[pag_index] = (physical_address & 0xFFFFF000) | (1 << 3) | (1 << 2) | ((writable & 1) << 1) | (present & 1);

    if (paging_enabled)
        pg_set_enabled(1);
}

