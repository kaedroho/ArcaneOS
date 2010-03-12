#ifndef MM_H_INCLUDED
#define MM_H_INCLUDED

struct memory_page_allocator
{
    unsigned long page_size;
    void* (*alloc)();
    void (*free)(void* ptr);
};

struct memory_block_allocator
{
    void* (*alloc)(unsigned int size);
    void (*free)(void* ptr);
};

struct memory_manager
{
    struct memory_page_allocator page_allocator;
    struct memory_block_allocator block_allocator;
};

extern void mm_init_stack_page_allocator();
extern void mm_init_pool_block_allocator();

extern void* mm_block_alloc(unsigned int size);
extern void mm_block_free(void* ptr);
extern void* mm_page_alloc();
extern void mm_page_free(void* ptr);
extern void* mm_physical_page_alloc();
extern void mm_physical_page_free(void* ptr);
extern void* mm_block_alloc(unsigned int size);
extern void mm_block_free(void* ptr);
extern void mm_init();

extern unsigned int mm_reserved_end;
extern unsigned int mm_page_size;

#endif // MM_H_INCLUDED
