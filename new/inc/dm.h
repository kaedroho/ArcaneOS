/* 
 * File:   driver_manager.h
 * Author: karl
 *
 * Created on 18 November 2010, 16:56
 */
#ifndef DRIVER_MANAGER_H
#define	DRIVER_MANAGER_H

struct dm_class
{
    char name[32];
};

struct dm_driver
{
    char name[32];
    struct dm_class* dclass;
    void* data;
};

struct dm_device
{
    char name[32];
    struct dm_driver* driver;
    void* data;
};


void dm_init();
void dm_register_class(struct dm_class* dclass);
void dm_unregister_class(struct dm_class* dclass);
void dm_register_driver(struct dm_driver* driver);
void dm_unregister_driver(struct dm_driver* driver);
void dm_register_device(struct dm_device* device);
void dm_unregister_device(struct dm_device* device);



//FILESYSTEM CLASS
struct dm_fs_driver
{
    void*(*openfs)(void* data);
    void(*closefs)(void* fsdata);
    void*(*openfile)(void* fsdata,char* name);
    void(*closefile)(void* filedata) ;
    unsigned int(*readfile)(void* file,void* buffer,unsigned int bytes);
    unsigned int(*writefile)(void* file,void* buffer,unsigned int bytes);

    void* data;
};
void dm_fs_init();
void dm_fs_register_driver(struct dm_fs_driver* driver,char* name);
void dm_fs_unregister_driver(struct dm_fs_driver* driver);


//INPUT/OUTPUT CLASS
struct dm_io_driver
{
    unsigned int(*read)(void* device,void* buffer,unsigned int bytes);
    unsigned int(*write)(void* device,void* buffer,unsigned int bytes);
    void* data;
};
void dm_io_init();
void dm_io_register_driver(struct dm_io_driver* driver,char* name);
void dm_io_unregister_driver(struct dm_io_driver* driver);

#endif	/* DRIVER_MANAGER_H */

