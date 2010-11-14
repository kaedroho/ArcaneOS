#include "x86/sys.h"

void mm_low_init();

extern unsigned char low_code, low_code_end, low_phys, phys;

void mm_init() {
    mm_low_init();
}

unsigned char* mm_low_base = 0;
unsigned char* mm_low_end = 0;

void mm_low_init() {
    // DESCRIPTION:
    // The low memory allocator uses a linked list of 'blocks'. Each block
    // starts with a 4 byte integer, the magnitude of which indicates the
    // offset to the next block. If the integer is positive, the block is
    // free; if it is negative, the block is used; and if it is zero, it
    // is the end of the linked list. The list is singly linked, meaning
    // that it can only be traversed from start to end.

    mm_low_base = &low_phys + (&low_code_end - &low_code);
    mm_low_end = (unsigned char*)0x80000 - sizeof(int);
    
    // Set the first block to be the size of the available memory and free
    *(int*)mm_low_base = mm_low_end - mm_low_base;
    // Set the last block to size zero
    *(int*)mm_low_end = 0;
}

void mm_low_free(unsigned char* mem) {
    // Ignore null pointers
    if (!mem)
        return;

    // Find start of block
    mem -= sizeof(int);

    // Set up variable for traversal through list
    unsigned char* ptr = mm_low_base;
    unsigned char* prev_ptr = 0;
    int block_size = 0;
    
    // Keep looping until the last block
    while ((block_size = *(int*)ptr)) {
        // If block is allocated
        if (block_size < 0) {
            unsigned char* next_ptr = ptr - block_size;

            // If we have found the block to be freed
            if (ptr == mem) {
                // If previous block is free
                if (prev_ptr && *(int*)prev_ptr > 0)
                    // Merge with previous block
                    ptr = prev_ptr;

                // If next block is free
                if (*(int*)next_ptr > 0)
                    // Merge with next block
                    next_ptr += *(int*)next_ptr;

                // Set this block to be free and the size of the new region
                *(int*)ptr = next_ptr - ptr;
                
                return;
            }

            // Advance to the next block
            ptr = next_ptr;
        }
        else
            ptr += block_size;
    }

    // INVALID MEMORY POINTER!
    // >> INSERT ERROR HERE <<
}

unsigned char* mm_low_alloc_aligned(unsigned size, unsigned alignment) {
    unsigned char* ptr = mm_low_base;
    unsigned char* prev_ptr = 0;
    int block_size = 0;

    // Alignment and size must be multiples of 4
    alignment = ((alignment+3)/4)*4;
    size = ((size+3)/4)*4;

    if (!alignment || !size)
        return 0;

    // Keep looping until the last block
    while ((block_size = *(int*)ptr)) {
        // If block is free
        if (block_size > 0) {
            unsigned char* next_ptr = ptr + block_size;
            // Align to 'alignment'
            unsigned char* loc = (unsigned char*)(((((unsigned)ptr)+sizeof(int)+alignment-1)/alignment)*alignment);
            unsigned char* loc_end = loc + size;
            // If there is enough space
            if (loc_end <= ptr + block_size) {
                // If there is a gap at the start, eliminate it
                if (loc > ptr+sizeof(int)) {
                    // If the previous block is not free, insert a new block after it
                    if (!prev_ptr || *(int*)prev_ptr < 0) {
                        *(int*)ptr = (loc-sizeof(int))-ptr;
                        ptr = loc-sizeof(int);
                    } else { // Else resize previous block to encompass gap
                        *(int*)prev_ptr = (loc-sizeof(int))-prev_ptr;
                        ptr = loc-sizeof(int);
                    }
                }
                // Set to used (< 0)
                *(int*)ptr = ptr-loc_end;
                // If there is a gap at the end, eliminate it
                if (loc_end < next_ptr) {
                    // If next block is not free insert a new block before it
                    if (*(int*)next_ptr <= 0) {
                        *(int*)loc_end = next_ptr - loc_end;
                    } else { // Else move and resize next block to encompass gap
                        *(int*)loc_end = next_ptr + *(int*)next_ptr - loc_end;
                    }
                }

                // Return new memory
                return loc;
            }

            ptr = next_ptr;
        }
        else
            ptr -= block_size;

        prev_ptr = ptr;
    }

    // Out of memory
    return 0;
}

unsigned char* mm_low_alloc(unsigned size) {
    return mm_low_alloc_aligned(size, 4);
}

void* mm_kernel_page_alloc(unsigned count) {
    void* ptr = pg_physical_page_alloc_range(count);
    
    unsigned i;
    for (i = 0; i < count; i++) {
        void* address = (void*)((unsigned)ptr + i*pg_page_size);
        pg_map_page(pg_kernel_directory, address, address, 2);
    }
    
    return ptr;
}

void mm_kernel_page_free(void* ptr, unsigned count) {
    unsigned i;
    for (i = 0; i < count; i++) {
        void* address = (void*)((unsigned)ptr + i*pg_page_size);
        pg_unmap_page(pg_kernel_directory, address);
    }

    pg_physical_page_free_range(ptr, count);
}

// Pools of size (index+1)*32 bytes
void* mm_kernel_pools[32] = {0};

void* mm_kernel_alloc(unsigned size) {
    if (size > 1024) {
        int pages = (size + sizeof(unsigned) + pg_page_size - 1)/pg_page_size;
        unsigned* ptr = (unsigned*)mm_kernel_page_alloc(pages);

        if (!ptr)
            return 0;

        *ptr++ = size;
        
        return ptr;
    } else {
        // Round up to multiple of 32 bytes
        size = ((size+31)/32)*32;
        // Calculate index into pools array
        int index = (size/32)-1;
        // Calculate length of bit-map in bytes
        int bitmap_length = ((pg_page_size-12+size-1)/size+7)/8;
        // Calculate number of bits in bit-map (number of blocks in pool)
        int bit_count = (pg_page_size-12-bitmap_length+size-1)/size;

        int i;
        void* pool;
        unsigned char* bitmap;

        // Loop through linked list of pools
        for (pool = mm_kernel_pools[index]; pool; pool = *((void**)pool+1))
            // If number of allocated blocks is less than number of blocks
            if (((int*)pool)[2] < bit_count) {
                // Increment number of allocated blocks
                ((int*)pool)[2]++;
                // Calculate offset of bit-map
                bitmap = (unsigned char*)pool + 12;
                // Loop through bits of bit-map
                for (i = 0; i < bit_count; i++)
                    // If bit is zero, set it to one, and return new block
                    if ((bitmap[i/8] & (1 << (i%8))) == 0) {
                        bitmap[i/8] |= (1 << (i%8));
                        return bitmap + bitmap_length + i*size;
                    }
            }

        // If no free blocks were found, allocate new pool
        pool = mm_kernel_page_alloc(1);

        // Out of memory
        if (!pool)
            return 0;

        // Set first dword of pool to the size of blocks in the pool
        ((int*)pool)[0] = size;
        // Set the second dword to point to the next pool in the list
        ((void**)pool)[1] = mm_kernel_pools[index];
        // Set the number of allocated chunks to one
        ((int*)pool)[2] = 1;

        // Set the newly allocated pool as the first in the list
        mm_kernel_pools[index] = pool;
        // Calculate offset of bit-map
        bitmap = (unsigned char*)pool + 12;

        // Set all bits to zero
        for (i = 0; i < bitmap_length; i++)
            bitmap[i] = 0;

        // Set first bit to one
        bitmap[0] = 1;

        // Return first block from pool
        return bitmap + bitmap_length;
    }
}

void mm_kernel_free(void* ptr) {
    // Find page containing ptr
    unsigned address = (unsigned)ptr & 0xFFFFF000;
    // Size of the block is stored at the start of the page
    unsigned size = *((unsigned*)address);
    if (size > 1024) {
        int pages = (size + sizeof(unsigned) + pg_page_size - 1)/pg_page_size;
        mm_kernel_page_free(ptr, pages);
    } else {
        // Calculate index into pools array
        int index = (size/32)-1;
        // Calculate length of bit-map in bytes
        int bitmap_length = ((pg_page_size-12+size-1)/size+7)/8;

        int i;
        void* pool = (void*)address;
        // Calculate address of bit-map
        unsigned char* bitmap = (unsigned char*)pool + 12;

        // Calculate index of block in pool
        i = ((unsigned char*)ptr - (bitmap + bitmap_length))/size;
        // Clear the bit representing this block
        bitmap[i/8] &= ~(unsigned char)(1 << (i%8));

        // Decrement the number of allocated blocks in the pool
        ((int*)pool)[2]--;

        // If the pool is empty
        if (!((int*)pool)[2]) {
            void* p;
            // Loop through linked list of pools
            for (p = mm_kernel_pools[index]; p; p = *((void**)p+1))
                // If we have found the pool before our one
                if (((void**)p)[1] == pool) {
                    // Set its next pointer to our pool's next pointer
                    ((void**)p)[1] = ((void**)pool)[1];
                    // Free our pool
                    mm_kernel_page_free(pool, 1);

                    return;
                }
            // If no previous pool was found we must be the first in the list

            // Set the first item of the list to the one after our one
            mm_kernel_pools[index] = ((void**)pool)[1];

            // Free our pool
            mm_kernel_page_free(pool, 1);

            return;
        }
    }
}