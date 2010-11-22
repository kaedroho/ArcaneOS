#include <dm.h>
#include <vfs.h>
#include <console.h>
#include <string.h>

extern struct dm_fs_driver devfs_fsdriver;

struct dm_class** dm_classlist=0;
unsigned int dm_classcount;

extern struct dm_class dm_fs_class;
extern struct dm_class dm_io_class;

void* mm_kernel_alloc(unsigned size);
void dm_init()
{
//Register built in classes
    dm_register_class(&dm_fs_class);
    dm_register_class(&dm_io_class);

//Mount DevFS
    if(vfs_open_filesystem(&devfs_fsdriver,0,"/devices")){
        console_puts_protected("DEVICE MANAGER: DevFS successfully mounted to /devices.\n");
    }
}


void mm_kernel_free(void* ptr);
void dm_register_class(struct dm_class* dclass)
{
    unsigned int i=0;
    for(i=0;i<dm_classcount;i++){
        if(dm_classlist[i]->exists==0){
            dm_classlist[i]=dclass;
            dm_classlist[i]->exists=1;
            console_puts_protected("DEVICE MANAGER: Registered class: ");console_puts_protected(dclass->name);console_putc_protected('\n');
            return;
        }
    }

    dm_classcount++;
    struct dm_class** tempclasslist=(struct dm_class**)mm_kernel_alloc(dm_classcount*sizeof(struct dm_class*));
    memcpy((unsigned char*)tempclasslist,(unsigned char*)dm_classlist,(dm_classcount-1)*sizeof(struct dm_class*));
    if(dm_classcount!=1)
        mm_kernel_free(dm_classlist);
    dm_classlist=tempclasslist;
    dm_classlist[dm_classcount-1]=dclass;
    dm_classlist[dm_classcount-1]->id=dm_classcount;

    console_puts_protected("DEVICE MANAGER: Registered class: ");console_puts_protected(dclass->name);console_putc_protected('\n');
}

void dm_unregister_class(struct dm_class* dclass)
{
    console_puts_protected("DEVICE MANAGER: Unregistered class: ");console_puts_protected(dclass->name);console_putc_protected('\n');
    dclass->exists=0;

    unsigned int oldclasscount=dm_classcount;
    while(dm_classlist[dm_classcount-1]->exists==0)
        dm_classcount--;

    if(dm_classcount!=oldclasscount){
        if(dm_classcount==0){
            mm_kernel_free(dm_classlist);
        }else{
            struct dm_class** tempclasslist=(struct dm_class**)mm_kernel_alloc(dm_classcount*sizeof(struct dm_class*));
            memcpy((unsigned char*)tempclasslist,(unsigned char*)dm_classlist,dm_classcount*sizeof(struct dm_class*));
            mm_kernel_free(dm_classlist);
            dm_classlist=tempclasslist;
        }
    }
}


void dm_register_driver(struct dm_driver* driver)
{

}

void dm_unregister_driver(struct dm_driver* driver)
{
    if(driver->dclass->unregister_driver!=0)
        driver->dclass->unregister_driver(driver->data);
}

void _dm_unregister_driver(struct dm_driver* driver)
{
    if(driver->id!=0){
        //Driver ID = driver->id-1
    }
}


void dm_register_device(struct dm_device* device)
{

}

void dm_unregister_device(struct dm_device* device)
{

}