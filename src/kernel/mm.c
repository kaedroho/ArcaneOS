#include "mm.h"
#include "boot.h"
#include "cli.h"
#include "paging.h"
#include "libs/memory.h"

extern void start();

unsigned int mm_reserved_end = 0x120000;
unsigned int mm_page_size = 4096;

struct memory_manager g_memory_manager;

void mm_init()
{
    // Use a stack based page allocator
    mm_init_stack_page_allocator();

    // Use a pool based block allocator
    mm_init_pool_block_allocator();
}
void* mm_page_kalloc(unsigned int count)
{
    return mm_page_alloc(pg_kernel_directory,count);
}
void mm_page_kfree(void* ptr, unsigned int count)
{
    mm_page_free(pg_kernel_directory,ptr,count);
}
void* mm_page_alloc(struct pg_directory* directory, unsigned int count)
{
    unsigned int virtual_address = pg_find_virtual_pages(directory,count);

    // Map page as present
    unsigned int i;

    for (i = 0; i < count; i++)
    {
        unsigned int physical_address = (unsigned int)g_memory_manager.page_allocator.alloc();
        pg_map_page(directory,physical_address,virtual_address + i*mm_page_size,1,1);
    }

    return (void*)virtual_address;
}
void mm_page_free(struct pg_directory* directory, void* ptr, unsigned int count)
{
    unsigned int virtual_address = ((unsigned int)ptr) & 0xFFFFF000;

    // Map page as not present
    unsigned int i;

    for (i = 0; i < count; i++)
    {
        unsigned int physical_address = pg_virtual_to_physical(directory,virtual_address + i*mm_page_size);

        pg_map_page(directory,physical_address,virtual_address,1,0);
        g_memory_manager.page_allocator.free((void*)physical_address);
    }
}
void* mm_physical_page_alloc()
{
    return g_memory_manager.page_allocator.alloc();
}
void mm_physical_page_free(void* ptr)
{
    g_memory_manager.page_allocator.free(ptr);
}
void* mm_block_alloc(unsigned int size)
{
    return g_memory_manager.block_allocator.alloc(size);
}
void mm_block_free(void* ptr)
{
    g_memory_manager.block_allocator.free(ptr);
}

// STACK-BASED PAGE ALLOCATOR

unsigned int* spa_stack_base;
unsigned int* spa_stack_ptr;
unsigned int spa_page_count;

void* mm_stack_page_allocator_alloc()
{
    unsigned int ptr = *(spa_stack_ptr--);
    return (void*)(mm_page_size * ptr);
}

void mm_stack_page_allocator_free(void* ptr)
{
    *(++spa_stack_ptr) = (unsigned int)ptr / mm_page_size;
}

void mm_init_stack_page_allocator()
{
    struct multiboot_information* boot_info = get_multiboot_info();

    unsigned int mem_top = ((boot_info->mem_upper+1024)*1024/mm_page_size);            // Usable memory end in page size bytes
    unsigned int mem_bottom = mm_reserved_end/mm_page_size;                            // Usable memory start in page size bytes

    unsigned int mem_stack = (mem_top-mem_bottom)*4;
    mem_stack = (mem_stack+mm_page_size-1)/mm_page_size;                                  // Number of page size bytes used for page stack

    mm_reserved_end += mem_stack*mm_page_size;

    spa_stack_base = (unsigned int*)(mem_bottom*mm_page_size);
    mem_bottom += mem_stack;
    spa_page_count = mem_top-mem_bottom;

    unsigned int page_index;
    spa_stack_ptr = spa_stack_base-1; // Set the stack pointer to the element before the first element in the stack

    if (boot_info->flags & 64)
    {
        struct multiboot_mmap* mmap = boot_info->mmap_addr;
        while(mmap < boot_info->mmap_addr + boot_info->mmap_length)
        {
            if ((mmap->type == 1) && (mmap->base_addr_high == 0) && (mmap->length_high == 0))
            {
                unsigned int page_start = (mmap->base_addr_low + mm_page_size - 1)/mm_page_size;
                unsigned int page_end = (mmap->base_addr_low + mmap->length_low)/mm_page_size;

                if (page_start < mm_reserved_end/mm_page_size)
                    page_start = mm_reserved_end/mm_page_size;

                //cli_puts("\nAdding pages "); cli_putu32(page_start,10);
                //cli_puts(" to "); cli_putu32(page_end,10);

                for (page_index = page_start; page_index < page_end; page_index++)
                    *(++spa_stack_ptr) = page_index;
            }

            mmap = (struct multiboot_mmap*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
        }
    }
    else
        for (page_index = mem_bottom; page_index < mem_top; page_index++)
            *(++spa_stack_ptr) = page_index;

    g_memory_manager.page_allocator.alloc = mm_stack_page_allocator_alloc;
    g_memory_manager.page_allocator.free = mm_stack_page_allocator_free;
}

#define POOL_COUNT(i) (((4096-16)/(4 << i))*((32 << i)-1)/(32 << i))

// Layout:
// unsigned int PoolIndex
// void* Previous
// void* Next
// unsigned int Used

struct pba_pool
{
    unsigned int pool;
    struct pba_pool* prev;
    struct pba_pool* next;
    unsigned int used;
    unsigned int bits[0];
};

struct pba_pool* pba_pools[9] = {0};

unsigned int pba_pools_count[9] = {
    POOL_COUNT(0),POOL_COUNT(1),POOL_COUNT(2),
    POOL_COUNT(3),POOL_COUNT(4),POOL_COUNT(5),
    POOL_COUNT(6),POOL_COUNT(7),POOL_COUNT(8)
    };

struct pba_pool* pba_find_free_pool(unsigned int pool)
{
    struct pba_pool* cur_pool;
    for (cur_pool = pba_pools[pool]; cur_pool && (cur_pool->used < pba_pools_count[pool]); cur_pool = cur_pool->next);
    return cur_pool;
}

unsigned int pba_find_free_block(struct pba_pool* cur_pool)
{
    unsigned int index;
    for (index = 0; cur_pool->bits[index] == 0xFFFFFFFF; index++);

    unsigned int bit;
    unsigned int mask = cur_pool->bits[index];

    for (bit = 0; (mask >> bit) & 1; bit++);

    index = index*32+bit;

    return index;
}

void pba_mark_block(struct pba_pool* cur_pool, unsigned int index, int used)
{
    if (used)
        cur_pool->bits[index/32] |= ((unsigned int)1) << (index%32);
    else
        cur_pool->bits[index/32] &= ~(((unsigned int)1) << (index%32));
}

struct pba_pool* pba_add_new_pool(unsigned int pool)
{
    struct pba_pool* cur_pool = (struct pba_pool*)mm_page_kalloc(1);
    memset((unsigned char*)cur_pool,0,mm_page_size);

    cur_pool->pool = pool;
    cur_pool->next = pba_pools[pool];

    if (pba_pools[pool])
        pba_pools[pool]->prev = cur_pool;

    pba_pools[pool] = cur_pool;

    return cur_pool;
}

void pba_delete_pool(struct pba_pool* cur_pool)
{
    if (cur_pool->prev)
        cur_pool->prev->next = cur_pool->next;
    else
        pba_pools[cur_pool->pool] = cur_pool->next;

    if (cur_pool->next)
        cur_pool->next->prev = cur_pool->prev;

    mm_page_kfree(cur_pool,1);
}

struct pba_pool* pba_get_pool_from_ptr(void* ptr)
{
    return (struct pba_pool*)(((unsigned int)ptr) & 0xFFFFF000);
}

unsigned int pba_get_block_index_from_ptr(struct pba_pool* cur_pool, void* ptr)
{
    unsigned int pool_end_ptr = ((unsigned int)cur_pool) + mm_page_size;
    unsigned int block_offset = pool_end_ptr-(unsigned int)ptr;
    unsigned int block_size = ((unsigned int)4) << cur_pool->pool;
    return block_offset/block_size-1;
}

void* pba_get_block_ptr_from_index(struct pba_pool* cur_pool, unsigned int index)
{
    unsigned int pool_end_ptr = ((unsigned int)cur_pool) + mm_page_size;
    unsigned int block_size = ((unsigned int)4) << cur_pool->pool;

    return (void*)(pool_end_ptr - block_size*(index+1));
}

unsigned int pba_get_pool_from_size(unsigned int size)
{
    unsigned int pool = 0;
    if (size <= 4) pool = 0;
    else if (size <= 8) pool = 1;
    else if (size <= 16) pool = 2;
    else if (size <= 32) pool = 3;
    else if (size <= 64) pool = 4;
    else if (size <= 128) pool = 5;
    else if (size <= 256) pool = 6;
    else if (size <= 512) pool = 7;
    else if (size <= 1024) pool = 8;

    return pool;
}

void* mm_pool_block_allocator_alloc(unsigned int size)
{
    unsigned int pool = pba_get_pool_from_size(size);
    size = ((unsigned int)4) << pool;

    struct pba_pool* pool_ptr = pba_find_free_pool(pool);
    if (!pool_ptr)
        pool_ptr = pba_add_new_pool(pool);

    unsigned int block = pba_find_free_block(pool_ptr);

    pba_mark_block(pool_ptr,block,1);
    pool_ptr->used++;

    return pba_get_block_ptr_from_index(pool_ptr,block);
}

void mm_pool_block_allocator_free(void* ptr)
{
    struct pba_pool* pool_ptr = pba_get_pool_from_ptr(ptr);
    unsigned int index = pba_get_block_index_from_ptr(pool_ptr,ptr);

    pba_mark_block(pool_ptr,index,0);
    pool_ptr->used--;

    if (pool_ptr->used == 0)
        pba_delete_pool(pool_ptr);
}

void mm_init_pool_block_allocator()
{
    g_memory_manager.block_allocator.alloc = mm_pool_block_allocator_alloc;
    g_memory_manager.block_allocator.free = mm_pool_block_allocator_free;
}

