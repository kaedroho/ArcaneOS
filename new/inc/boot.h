
#ifndef BOOT_H
#define	BOOT_H

//MULTIBOOT STRUCTURES

struct multiboot_device
{
    unsigned char part3;
    unsigned char part2;
    unsigned char part1;
    unsigned char drive;
} __attribute__((packed));

struct multiboot_module
{
    unsigned int mod_start;
    unsigned int mod_end;
    char* string;
    unsigned int reserved; // Always 0
} __attribute__((packed));

struct multiboot_symboltable
{
    unsigned int tabsize;
    unsigned int strsize;
    unsigned int addr;
    unsigned int reserved; // Always 0
} __attribute__((packed));

struct multiboot_sectiontable
{
    unsigned int num;
    unsigned int size;
    unsigned int addr;
    unsigned int shndx;
} __attribute__((packed));

struct multiboot_drive
{
    unsigned int size;
    unsigned char drive_number;
    unsigned char drive_mode;
    unsigned char drive_cylinders;
    unsigned char drive_heads;
    unsigned char drive_sectors;
    unsigned short drive_ports[0];
} __attribute__((packed));

struct multiboot_apmtable
{
    unsigned short version;
    unsigned short cseg;
    unsigned int offset;
    unsigned short cseg_16;
    unsigned short dseg;
    unsigned short flags;
    unsigned short cseg_len;
    unsigned short cseg_16_len;
    unsigned short dseg_len;
} __attribute__((packed));

struct multiboot_mmap
{
    unsigned long size;
    unsigned long base_addr_low;
    unsigned long base_addr_high;
    unsigned long length_low;
    unsigned long length_high;
    unsigned long type;
};

struct multiboot_information
{
    unsigned int flags;
    // ---------------------------------------------------------
    unsigned int mem_lower; // present if flags[0] is set
    unsigned int mem_upper; //present if flags[0] is set
    // ---------------------------------------------------------
    struct multiboot_device boot_device; //present if flags[1] is set
    // ---------------------------------------------------------
     char* cmdline; //present if flags[2] is set
    // ---------------------------------------------------------
    unsigned int mods_count; //present if flags[3] is set
    struct multiboot_module* mods_addr; //present if flags[3] is set
    // ---------------------------------------------------------
    union {
        struct multiboot_symboltable symboltable; //present if flags[4] is set
        struct multiboot_sectiontable sectiontable; //present if flags[5] is set
    } __attribute__((packed));
    // ---------------------------------------------------------
    unsigned int mmap_length; //present if flags[6] is set
    struct multiboot_mmap* mmap_addr; //present if flags[6] is set
    // ---------------------------------------------------------
    unsigned int drives_length; //present if flags[7] is set
    struct multiboot_drive* drives_addr; //present if flags[7] is set
    // ---------------------------------------------------------
    unsigned int config_table; //present if flags[8] is set
    // ---------------------------------------------------------
    char* boot_loader_name; //present if flags[9] is set
    // ---------------------------------------------------------
    struct multiboot_apmtable* apm_table; //present if flags[10] is set
    // ---------------------------------------------------------
    unsigned int vbe_control_info; //present if flags[11] is set
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_len;

} __attribute__((packed));

extern struct multiboot_information* get_multiboot_info();

#define MB_GET_FLAG(flag, index) (((flag >> index) & 1) != 0)

#endif

