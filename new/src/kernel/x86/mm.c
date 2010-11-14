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
