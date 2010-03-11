#include "paging.h"
#include "mm.h"
#include "libs/memory.h"

struct pg_directory* pg_kernel_directory = 0;

void pg_init()
{
    pg_kernel_directory = pg_alloc_directory();
    pg_map_page(pg_kernel_directory,(unsigned int)pg_kernel_directory,(unsigned int)pg_kernel_directory,1,1);

    // Map reserved memory directly from virtual to physical
    unsigned int ptr;
    for (ptr = 0; ptr < mm_reserved_end; ptr += 4096)
        pg_map_page(pg_kernel_directory,ptr,ptr,1,1);

    // Enable paging
    pg_set_directory(pg_kernel_directory);
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
    struct pg_directory* dir = (struct pg_directory*)mm_page_alloc();
    memset((unsigned char*)dir,0,4096);
    return dir;
}
void pg_free_directory(struct pg_directory* directory)
{
    mm_page_free(directory);
}
struct pg_table* pg_alloc_table()
{
    struct pg_table* tbl = (struct pg_table*)mm_page_alloc();
    memset((unsigned char*)tbl,0,4096);
    return tbl;
}
void pg_free_table(struct pg_table* table)
{
    mm_page_free(table);
}
void pg_map_page(struct pg_directory* directory, unsigned int physical_address, unsigned int virtual_address, unsigned int writable, unsigned int present)
{
    unsigned int dir_index = (virtual_address >> 22) & 0x3FF;
    unsigned int pag_index = (virtual_address >> 12) & 0x3FF;

    struct pg_table* table = directory->tables[dir_index];

    if (!table)
    {
        // Save current directory
        struct pg_directory* old_dir = pg_get_directory();

        // Disable paging
        pg_set_directory(0);

        // Allocate new table
        table = directory->tables[dir_index] = pg_alloc_table();

        // Make sure page is accessible to the current directory
        pg_map_page(old_dir, (unsigned int)table, (unsigned int)table, 1, 1);

        // Enable paging
        pg_set_directory(old_dir);
    }

    // Map page
    table->pages[pag_index] = (physical_address & 0xFFFFF000) | (1 << 2) | ((writable & 1) << 1) | (present & 1);
}

