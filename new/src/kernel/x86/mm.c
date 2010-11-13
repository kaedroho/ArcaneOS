void mm_low_init();

extern unsigned char low_code, low_code_end, low_phys, phys;

void mm_init() {
    mm_low_init();
}


unsigned char* mm_low_base = 0;
unsigned char* mm_low_end = 0;

void mm_low_init() {
    mm_low_base = &low_phys + (&low_code_end - &low_code);
    mm_low_end = &phys - sizeof(int);
    
    *(int*)mm_low_base = mm_low_end - mm_low_base;
    *(int*)mm_low_end = 0;
}

void mm_low_free(unsigned char* mem) {
    if (!mem)
        return;

    mem -= sizeof(int);

    unsigned char* ptr = mm_low_base;
    unsigned char* prev_ptr = 0;
    int block_size = 0;
    
    // Keep looping until the last block
    while ((block_size = *(int*)ptr)) {
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

                *(int*)ptr = next_ptr - ptr;
                
                return;
            }

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

    return 0;
}

unsigned char* mm_low_alloc(unsigned size) {
    return mm_low_alloc_aligned(size, 4);
}
