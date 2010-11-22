#include <dm.h>
#include <string.h>

struct dm_class dm_io_class={
    .name="io"
};

void* mm_kernel_alloc(unsigned size);
void dm_io_register_driver(struct dm_io_driver* driver,char* name)
{
    struct dm_driver* dmdriver=mm_kernel_alloc(sizeof(struct dm_driver));
    strcpy(dmdriver->name,name);
    dmdriver->dclass=&dm_io_class;
    dmdriver->data=(void*)driver;
    driver->data=(void*)dmdriver;
    dm_register_driver(dmdriver);
}

void mm_kernel_free(void* ptr);
void _dm_unregister_driver(struct dm_driver* driver);
void dm_io_unregister_driver(struct dm_io_driver* driver)
{
    if(driver->data!=0){
        struct dm_driver* dmdriver=(struct dm_driver*)driver->data;
        _dm_unregister_driver(dmdriver);
        mm_kernel_free((void*)dmdriver);
        driver->data=0;
    }
}

void _dm_io_unregister_driver(void* driver)
{
    dm_io_unregister_driver((struct dm_io_driver*)driver);
}