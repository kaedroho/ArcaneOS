#ifndef SYS_X86_H
#define	SYS_X86_H

#define MAGIC_BREAKPOINT __asm__ ("XCHG %BX, %BX");

//SYSTEM FUNCTIONS
static inline unsigned char inb(unsigned short port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "d" (port));
    return rv;
}

static inline void outb(unsigned short port,unsigned char data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "d" (port), "a" (data));
}

static inline unsigned short inw(unsigned short port)
{
    unsigned short rv;
    __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "d" (port));
    return rv;
}

static inline void outw(unsigned short port,unsigned short data)
{
    __asm__ __volatile__ ("outw %1, %0" : : "d" (port), "a" (data));
}
/*
static inline unsigned long inl(unsigned short port)
{
    unsigned long rv;
    __asm__ __volatile__ ("inl %1, %0" : "=a" (rv) : "d" (port));
    return rv;
}
*/
static inline void outl(unsigned short port,unsigned long data)
{
    __asm__ __volatile__ ("outl %1, %0" : : "d" (port), "a" (data));
}

static inline unsigned char peekb(unsigned short seg,unsigned short off)
{
    return *(unsigned char*)(seg*16+off);
}

static inline void pokeb(unsigned short seg,unsigned short off,unsigned char val)
{
    *(unsigned char*)(seg*16+off)=val;
}

static inline unsigned short peekw(unsigned short seg,unsigned short off)
{
    return *(unsigned short*)(seg*16+off);
}

static inline void pokew(unsigned short seg,unsigned short off,unsigned short val)
{
    *(unsigned short*)(seg*16+off)=val;
}

static inline unsigned long peekl(unsigned short seg,unsigned short off)
{
    return *(unsigned long*)(seg*16+off);
}

static inline void pokel(unsigned short seg,unsigned short off,unsigned long val)
{
    *(unsigned long*)(seg*16+off)=val;
}


//GDT FUNCTIONS
extern void gdt_init();
extern void gdt_set_gate(int seg,unsigned long base,unsigned long limit,unsigned char access,unsigned char gran);

//IDT FUNCTIONS
extern void idt_init();
extern void idt_set_gate(unsigned char num,unsigned long base,unsigned short sel,unsigned char flags);

//REGISTER STRUCTURE (FOR USE WITH INTERRUPTS)
struct regs
{
    // Kernel and user mode values
    unsigned int new_esp;
    unsigned int gs,fs,es,ds;
    unsigned int edi,esi,ebp,esp,ebx,edx,ecx,eax;
    unsigned int int_no,err_code;
    unsigned int eip,cs,eflags;
    union {
        struct {
            unsigned int kernel_return_address, kernel_param;
        };
    // User mode only values
        struct {
            unsigned int useresp, ss;
            unsigned int usermode_return_address, usermode_param;
        };
    };
};


//ISR FUNCTIONS
extern void isr_init();


//IRQ FUNCTIONS
extern void irq_init();
extern void irq_install_handler(int irq,void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_remap(void);
extern void irq_unmap(void);

extern unsigned int irq_lock();
extern void irq_unlock(unsigned int handle);
extern unsigned int irq_query();
extern struct regs* irq_regs;



//CPUID STRUCTURE
struct cpuid_struct
{
    unsigned int maxstandardlevel;                  //0000_0000h (EAX)
    char vendor[13];                                //0000_0000h (EBX ECX EDX)

    unsigned int processorinfoa;                    //0000_0001h (EAX)
    unsigned int processorinfob;                    //0000_0001h (EBX)
    unsigned int processorinfoc;                    //0000_0001h (ECX)
    unsigned int processorinfod;                    //0000_0001h (EDX)

    unsigned int l1cacheconfiga;                    //0000_0004h (EAX)
    unsigned int l1cacheconfigb;                    //0000_0004h (EBX)
    unsigned int l1cacheconfigc;                    //0000_0004h (ECX)
    unsigned int l1cacheconfigd;                    //0000_0004h (EDX)

    unsigned int l2cacheconfiga;                    //0000_0004h (EAX)
    unsigned int l2cacheconfigb;                    //0000_0004h (EBX)
    unsigned int l2cacheconfigc;                    //0000_0004h (ECX)
    unsigned int l2cacheconfigd;                    //0000_0004h (EDX)

    unsigned int moninfoa;                          //0000_0005h (EAX)
    unsigned int moninfob;                          //0000_0005h (EBX)
    unsigned int moninfoc;                          //0000_0005h (ECX)
    unsigned int moninfod;                          //0000_0005h (EDX)

    unsigned int powermanagementinfoa;              //0000_0006h (EAX)
    unsigned int powermanagementinfob;              //0000_0006h (EBX)
    unsigned int powermanagementinfoc;              //0000_0006h (ECX)

    unsigned int dcaparams;                         //0000_0009h (EAX)

    unsigned int architecturalpemoinfoa;            //0000_000Ah (EAX)
    unsigned int architecturalpemoinfob;            //0000_000Ah (EBX)
    unsigned int architecturalpemoinfod;            //0000_000Ah (EDX)
} __attribute__((packed));

//CPUID FUNCTIONS
extern void cpuid_init();
extern struct cpuid_struct* cpuid_getstruct();


//TIMER FUNCTIONS
extern void timer_init();
extern unsigned int timer_getticks();
extern unsigned int timer_getticksperseccond();


//CMOS FUNCTIONS
extern void cmos_init();
extern unsigned char cmos_get(unsigned char reg);
extern void cmos_set(unsigned char reg,unsigned char value);


//PAGING STRUCTURES
struct pg_page_directory
{
    unsigned int tables[1024];
};

struct pg_page_table
{
    unsigned int pages[1024];
};

//PAGING FUNCTIONS
void pg_init();
void pg_map_page(struct pg_page_directory* directory, void* virtual, void* physical, unsigned attr);
void pg_unmap_page(struct pg_page_directory* directory, void* virtual);
void pg_set_directory(struct pg_page_directory* directory);
void pg_physical_page_free(void* address);
void pg_physical_page_free_range(void* address, int count);
void pg_physical_page_reserve(void* address);
void pg_physical_page_reserve_range(void* address, int count);
int pg_is_physical_page_free(void* address);
void* pg_physical_page_alloc();
void* pg_physical_page_alloc_range(unsigned count);
void pg_find_pages();
void pg_init_directory(struct pg_page_directory* directory);
void pg_init_table(struct pg_page_table* table);
void pg_enable_paging();
void pg_disable_paging();
int pg_is_paging_enabled();
void pg_push_state();
void pg_pop_state();

extern struct pg_page_directory* pg_kernel_directory;
extern struct pg_page_directory* pg_current_directory;
extern unsigned int pg_page_size;

// LOWMEM FUNCTIONS
void lm_init();

// MEMORY FUNCTIONS
void mm_init();
void mm_low_free(unsigned char* mem);
unsigned char* mm_low_alloc_aligned(unsigned size, unsigned alignment);
unsigned char* mm_low_alloc(unsigned size);

void* mm_kernel_page_alloc(unsigned count);
void mm_kernel_page_free(void* ptr, unsigned count);
void* mm_kernel_alloc(unsigned size);
void mm_kernel_free(void* ptr);

#endif