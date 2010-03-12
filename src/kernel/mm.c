#include "mm.h"
#include "boot.h"
#include "cli.h"
#include "paging.h"

extern void start();

unsigned int mm_reserved_end = 0x120000;

struct memory_manager g_memory_manager;

void mm_init()
{
    // Use a stack based page allocator
    mm_init_stack_page_allocator();
}
void* mm_page_alloc()
{
    void* ptr = g_memory_manager.page_allocator.alloc();

    // Map page as present
    pg_map_page(pg_kernel_directory,(unsigned int)ptr,(unsigned int)ptr,1,1);

    return ptr;
}
void mm_page_free(void* ptr)
{
    // Map page as not present
    pg_map_page(pg_kernel_directory,(unsigned int)ptr,(unsigned int)ptr,1,0);

    g_memory_manager.page_allocator.free(ptr);
}
void* mm_physical_page_alloc()
{
    return g_memory_manager.page_allocator.alloc();
}
void mm_physical_page_free(void* ptr)
{
    g_memory_manager.page_allocator.free(ptr);
}

// STACK-BASED PAGE ALLOCATOR

unsigned int* spa_stack_base;
unsigned int* spa_stack_ptr;
unsigned int spa_page_count;

void* mm_stack_page_allocator_alloc()
{
    unsigned int ptr = *(spa_stack_ptr--);
    return (void*)(g_memory_manager.page_allocator.page_size * ptr);
}

void mm_stack_page_allocator_free(void* ptr)
{
    *(++spa_stack_ptr) = (unsigned int)ptr / g_memory_manager.page_allocator.page_size;
}

void mm_init_stack_page_allocator()
{
    unsigned int page_size = 1024*4; // 4KB page size

    struct multiboot_information* boot_info = get_multiboot_info();

    unsigned int mem_top = ((boot_info->mem_upper+1024)*1024/page_size);            // Usable memory end in page size bytes
    unsigned int mem_bottom = mm_reserved_end/page_size;                            // Usable memory start in page size bytes

    unsigned int mem_stack = (mem_top-mem_bottom)*4;
    mem_stack = (mem_stack+page_size-1)/page_size;                                  // Number of page size bytes used for page stack

    mm_reserved_end += mem_stack*page_size;

    spa_stack_base = (unsigned int*)(mem_bottom*page_size);
    mem_bottom += mem_stack;
    spa_page_count = mem_top-mem_bottom;

    unsigned int page_index;
    spa_stack_ptr = spa_stack_base-1; // Set the stack pointer to the element before the first element in the stack

    for (page_index = mem_bottom; page_index < mem_top; page_index++)
        *(++spa_stack_ptr) = page_index;

    g_memory_manager.page_allocator.page_size = page_size;
    g_memory_manager.page_allocator.alloc = mm_stack_page_allocator_alloc;
    g_memory_manager.page_allocator.free = mm_stack_page_allocator_free;
}
