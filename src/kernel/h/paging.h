#ifndef PAGING_H_INCLUDED
#define PAGING_H_INCLUDED

extern void _paging_stack();

struct pg_table
{
    unsigned int pages[0];
} __attribute__((packed));

struct pg_directory
{
    unsigned int tables[0];
} __attribute__((packed));

extern struct pg_directory* pg_kernel_directory;

extern void pg_init();
/*
extern void pg_set_enabled(int enabled);
extern int pg_get_enabled();
*/
extern void pg_set_directory(struct pg_directory* directory);
extern struct pg_directory* pg_get_directory();
extern struct pg_directory* pg_alloc_directory();
extern void pg_free_directory(struct pg_directory* directory);
extern struct pg_table* pg_alloc_table();
extern void pg_free_table(struct pg_table* table);

extern void pg_map_page(struct pg_directory* directory, unsigned int physical_address, unsigned int virtual_address, unsigned int writable, unsigned int present);

extern unsigned int pg_find_virtual_pages(struct pg_directory* directory, unsigned int count);
extern unsigned int pg_virtual_to_physical(struct pg_directory* directory, unsigned int virtual_address);
/*
extern int pg_lock();
extern void pg_unlock(int handle);
*/
#endif // PAGING_H_INCLUDED
