#include <x86/ibmpc.h>
#include <x86/boot.h>
#include <console.h>
#include <string.h>
#include <vfs.h>

extern struct vfs_filesystem_driver initrd_fsdriver;

void ibmpc_init()
{
//Initialise user input
    kb_init();

//Initialise clock
    ibmpc_initclock();

//Initialise filesystems
    initrd_init();

//Load initrd
    struct multiboot_information* mboot=get_multiboot_info();
    unsigned int i=0;
    void* initrd_ptr=0;
    for(i=0;i<mboot->mods_count;i++){
        if(strcmp(mboot->mods_addr[i].string,"(fd0)/init.rd")){
            console_puts_protected("INITRD: Found image at: 0x");
            console_putu32_protected(mboot->mods_addr[i].mod_start,16);
            initrd_ptr=(void*)mboot->mods_addr[i].mod_start;
            console_putc_protected('\n');
        }
    }

//Mount initrd
    if(initrd_ptr!=0){
        struct vfs_filesystem* initfs=vfs_open_filesystem(&initrd_fsdriver,initrd_ptr,"/init");
        if(initfs!=0)
            console_puts_protected("INITRD: Image successfully mounted to /init.\n");
        else
            console_puts_protected("INITRD: Error. Could not mount image to /init.\n");
    }
}