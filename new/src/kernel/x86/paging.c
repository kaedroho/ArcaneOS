#include <sys-x86.h>
#include <boot.h>
#include <string.h>
#include <console.h>

unsigned int pg_kernelreserved;

unsigned int* pg_page_stack_ptr;
unsigned int pg_page_size;
unsigned int pg_total_free_memory;

struct pg_pagedirectory* pg_kernel_directory;

void pg_findpages();
void* pg_physical_page_alloc();
void pg_physical_page_free(void* ptr);

void pg_init()
{
//Find free pages
    pg_findpages();
}

void pg_findpages()
{
//Setup variables
    pg_kernelreserved=0x20000;
    pg_page_stack_ptr=(unsigned int*)(0x100000+pg_kernelreserved);
    pg_page_size=0x1000;
    pg_total_free_memory=0;

//Get multiboot info
    struct multiboot_information* mboot=get_multiboot_info();

//TEMP: Find area which starts at 0x100000
    unsigned int i;
    unsigned int pagecount=0;
    for(i=0;i<mboot->mmap_length/24;i++)
    {
        if(mboot->mmap_addr[i].base_addr_low==0x100000)
        {
        //Work out number of pages to allocate
            unsigned int length=mboot->mmap_addr[i].length_low-pg_kernelreserved;
            pagecount=length>>12;

        //Allocate pages
            unsigned int page;
            for(page=pagecount;page>0;page--)
            {
                pg_physical_page_free((void*)(page<<12)+(0x100000+pg_kernelreserved));
            }
        }
    }

//Print to console
    console_puts_protected("PAGING: ");
    console_putu32_protected(pagecount,10);
    console_puts_protected(" pages allocated (");
    console_putu32_protected(pagecount*4096,10);
    console_puts_protected(" bytes).\n");

//Allocate kernel memory
    pg_kernel_directory=pg_alloc_directory();

}


struct pg_pagedirectory* pg_alloc_directory()
{
//Check that some memory is available
    if(pg_total_free_memory==0)
        return 0;

//Get a page
    void* address=pg_physical_page_alloc();

//Clean the page
    memset((unsigned char*)address,0,pg_page_size);

//Return the address
    return address;
}


void* pg_alloc_page(struct pg_pagedirectory* directory)
{
//Check that some memory is available
    if(pg_total_free_memory==0)
        return 0;

//Get a page
    void* address=pg_physical_page_alloc();

//Clean the page
    memset((unsigned char*)address,0,pg_page_size);

//Return the address
    return address;
}

void pg_addpagetodirectory(struct pg_pagedirectory* directory,void* page)
{

}

void* pg_physical_page_alloc()
{
    pg_total_free_memory-=pg_page_size;
    unsigned int ptr=*(pg_page_stack_ptr--);
    return (void*)(pg_page_size*ptr);
}

void pg_physical_page_free(void* ptr)
{
    pg_total_free_memory+=pg_page_size;
    *(++pg_page_stack_ptr)=(unsigned int)ptr/pg_page_size;
}