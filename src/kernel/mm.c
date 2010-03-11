#include "mm.h"
#include "boot.h"
#include "cli.h"

extern void start();

struct memory_manager g_memory_manager;

void* mm_page_alloc(unsigned int size)
{
    return g_memory_manager.page_allocator.alloc(size);
}
void mm_page_free(void* ptr)
{
    g_memory_manager.page_allocator.free(ptr);
}

// STACK-BASED PAGE ALLOCATOR

unsigned int* spa_stack_base;
unsigned int* spa_stack_ptr;
unsigned int spa_page_count;

void* mm_stack_page_allocator_alloc(unsigned int size)
{
    return (void*)(g_memory_manager.page_allocator.page_size * *(spa_stack_ptr--));
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
    unsigned int mem_bottom = ((unsigned int)&start + 0x10000 + page_size-1)/page_size;  // Usable memory start in page size bytes

    unsigned int mem_stack = (mem_top-mem_bottom)*4;
    mem_stack = (mem_stack+page_size-1)/page_size;                                  // Number of page size bytes used for page stack

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
