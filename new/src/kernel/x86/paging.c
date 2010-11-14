#include <x86/sys.h>
#include <x86/boot.h>
#include <string.h>
#include <console.h>

unsigned int* pg_page_bitmap;
unsigned int pg_page_bitmap_size;
unsigned int pg_page_bitmap_length;
unsigned int pg_page_size;

struct pg_page_directory* pg_kernel_directory;
struct pg_page_directory* pg_current_directory;

extern char end;

unsigned pg_stack = 0;

void pg_push_state() {
    pg_stack <<= 1;
    pg_stack |= pg_is_paging_enabled();
}
void pg_pop_state() {
    if (pg_stack & 1)
        pg_enable_paging();
    else
        pg_disable_paging();

    pg_stack >>= 1;
}

void pg_init() {
//Find end of kernel
    struct multiboot_information* mboot=get_multiboot_info();
    unsigned int i=0;
    unsigned int kernend=(unsigned int)&end;
    for(i=0;i<mboot->mods_count;i++){
        if(mboot->mods_addr[i].mod_end>kernend){
            kernend=mboot->mods_addr[i].mod_end;
        }
    }

//Paging variables
    pg_page_size=0x1000;
    pg_page_bitmap = (unsigned int*)(((kernend + pg_page_size-1)/pg_page_size)*pg_page_size);
    pg_page_bitmap_size = 0;

//Find free pages
    pg_find_pages();
    
//Allocate kernel directory
    pg_kernel_directory = pg_physical_page_alloc();
    pg_init_directory(pg_kernel_directory);

//Map kernel pages
    unsigned int page;
    unsigned range = (unsigned)pg_page_bitmap / pg_page_size + pg_page_bitmap_size;
    for(page = 0; page < range; page++) {
        void* address=(void*)(page*pg_page_size);
        pg_map_page(pg_kernel_directory, address, address, 2);
    }

//Set to the kernel directory
    pg_set_directory(pg_kernel_directory);

}

void pg_map_page(struct pg_page_directory* directory, void* virtual, void* physical, unsigned attr) {
    attr |= 1;

    unsigned virtual_address = (unsigned)virtual;
    unsigned directory_offset = virtual_address >> 22;
    unsigned table_offset = (virtual_address >> 12) & 0x3FF;
    
    if (!directory->tables[directory_offset]) {
        void* ptr = pg_physical_page_alloc();

        directory->tables[directory_offset] = (unsigned)ptr|3;

        pg_init_table((struct pg_page_table*)ptr);
    }

    struct pg_page_table* table = (struct pg_page_table*)(directory->tables[directory_offset] & 0xFFFFF000);
    table->pages[table_offset] = (unsigned)physical|attr;
}

void pg_unmap_page(struct pg_page_directory* directory, void* virtual) {

    unsigned virtual_address = (unsigned)virtual;
    unsigned directory_offset = virtual_address >> 22;
    unsigned table_offset = (virtual_address >> 12) & 0x3FF;

    if (!directory->tables[directory_offset])
        return;
    
    struct pg_page_table* table = (struct pg_page_table*)(directory->tables[directory_offset] & 0xFFFFF000);
    table->pages[table_offset] = 0;
    
    int i;
    for (i = 0; i < 1024; i++)
        if (table->pages[i])
            return;
    
    // Free table
    directory->tables[directory_offset] = 0;
    pg_unmap_page(pg_kernel_directory, table);
    pg_physical_page_free(table);
}

void pg_set_directory(struct pg_page_directory* directory) {
    pg_current_directory=directory;
    asm volatile("mov %0, %%cr3":: "r"(directory));
}

void pg_physical_page_free(void* address) {
    unsigned page = (unsigned)address / pg_page_size - 256;
    pg_page_bitmap[page/32] |= (1u << (page % 32));
}

void pg_physical_page_reserve(void* address) {
    unsigned page = (unsigned)address / pg_page_size - 256;
    pg_page_bitmap[page/32] &= ~(1u << (page % 32));
}

void pg_physical_page_reserve_range(void* address, int count) {
    int i;
    for (i = 0; i < count; i++)
        pg_physical_page_reserve((void*)((unsigned)address + i*pg_page_size));
}

int pg_is_physical_page_free(void* address) {
    unsigned page = (unsigned)address / pg_page_size - 256;
    return pg_page_bitmap[page/32] & (1u << (page % 32)) ? 1 : 0;
}

void* pg_physical_page_alloc() {
    int i, j;
    for (i = 0; i < pg_page_bitmap_length; i++)
        if (pg_page_bitmap[i]) {
            // Find highest bit
            asm ( "bsr %1, %0\n" : "=r"(j) : "r" (pg_page_bitmap[i]));

            void* ptr = (void*)((i*32 + j + 256)*pg_page_size);
            pg_physical_page_reserve(ptr);

            return ptr;
        }
    
    return 0;
}

void* pg_physical_page_alloc_range(unsigned count) {
    int i, j, k;
    for (i = 0; i < pg_page_bitmap_length; i++)
        if (pg_page_bitmap[i]) {
            unsigned start = (i*32 + 256)*pg_page_size;
            // For each possible offset
            for (j = 0; j < 32; j++) {
                // Assume all pages are free
                int all_free = 1;
                // Loop through all desired pages
                for (k = 0; k < count; k++) {
                    // If a page is not free, move on
                    if (!pg_is_physical_page_free((void*)(start + (j+k)*pg_page_size))) {
                        all_free = 0;
                        // Skip until next free page
                        j += k;
                        break;
                    }
                }
                // If all pages were free, reserve them
                if (all_free) {
                    void* ptr = (void*)(start + j*pg_page_size);
                    pg_physical_page_reserve_range(ptr, count);

                    return ptr;
                }
            }
        }

    return 0;
}

void pg_find_pages() {
    //Get multiboot info
    struct multiboot_information* mboot=get_multiboot_info();

    unsigned int i;
    unsigned int page_count=0;

    // Initialise page bitmap to zero
    // 0x8000 is its maximum possible size in dwords on a 32-bit system
    for (i = 0; i < 0x8000; i++)
        pg_page_bitmap[i] = 0;
    for(i=0;i<mboot->mmap_length/24;i++)
    {
        /*
        // USEFUL CODE TO DISPLAY MEMORY MAP
        //
        console_puts_protected("0x");
        console_putu32_protected(mboot->mmap_addr[i].base_addr_low,16);
        console_puts_protected(" -> 0x");
        console_putu32_protected(mboot->mmap_addr[i].base_addr_low + mboot->mmap_addr[i].length_low,16);
        console_puts_protected(" : ");
        console_putu32_protected(mboot->mmap_addr[i].type,16);
        console_puts_protected("\n");
         */

        // If region is available
        if(mboot->mmap_addr[i].type==1 && mboot->mmap_addr[i].base_addr_low >= 0x100000)
        {
            // Get start and end address
            unsigned page_start = mboot->mmap_addr[i].base_addr_low;
            unsigned page_end = page_start + mboot->mmap_addr[i].length_low;

            // Ignore regions below 1mb (the low memory allocator controls them)
            if (page_start < (unsigned)pg_page_bitmap)
                page_start = (unsigned)pg_page_bitmap;

            // Find the largest region of pages contained by the address range
            page_start = ((page_start + pg_page_size-1)/pg_page_size)*pg_page_size;
            page_end = (page_end/pg_page_size)*pg_page_size;

            if (page_end > page_start) {
                // Update length of page bitmap
                unsigned new_size = ((((page_end/pg_page_size - 256)+7)/8) + pg_page_size-1)/pg_page_size;
                pg_page_bitmap_size = pg_page_bitmap_size > new_size ? pg_page_bitmap_size : new_size;

                //Allocate pages
                unsigned page;
                for(page = page_start; page < page_end; page += pg_page_size) {
                    pg_physical_page_free((void*)page);
                    page_count++;
                }
            }
        }
    }

    // Subtract pages used by page bitmap
    page_count -= pg_page_bitmap_size;
    
    // Reserve pages used by page bitmap
    for (i = 0; i < pg_page_bitmap_size; i++)
        pg_physical_page_reserve((void*)((unsigned)pg_page_bitmap + i*pg_page_size));


    // Calculate the length of the page bitmap
    pg_page_bitmap_length = pg_page_bitmap_size*pg_page_size/sizeof(unsigned);

//Print to console
    console_puts_protected("PAGING: ");
    console_putu32_protected(page_count,10);
    console_puts_protected(" pages available (");
    console_putu32_protected(page_count*4096,10);
    console_puts_protected(" bytes).\n");
}

void pg_init_directory(struct pg_page_directory* directory) {
    if (pg_is_paging_enabled()) {
        pg_disable_paging();
        memset((unsigned char*)directory,0,pg_page_size);
        pg_map_page(pg_kernel_directory, directory, directory, 2);
        pg_enable_paging();
    } else {
        memset((unsigned char*)directory,0,pg_page_size);
        pg_map_page(pg_kernel_directory, directory, directory, 2);
    }
}

void pg_init_table(struct pg_page_table* table) {
    if (pg_is_paging_enabled()) {
        pg_disable_paging();
        memset((unsigned char*)table,0,pg_page_size);
        pg_map_page(pg_kernel_directory, table, table, 2);
        pg_enable_paging();
    } else {
        memset((unsigned char*)table,0,pg_page_size);
        pg_map_page(pg_kernel_directory, table, table, 2);
    }
}

