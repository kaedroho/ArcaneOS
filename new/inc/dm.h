/* 
 * File:   driver_manager.h
 * Author: karl
 *
 * Created on 18 November 2010, 16:56
 */
#ifndef DEVICE_MANAGER_H
#define	DEVICE_MANAGER_H

struct dm_driver;
struct dm_class
{
    char exists;
    char name[32];
    unsigned int id;
    void(*unregister_driver)(void* data);
    struct dm_driver** driverlist;
    unsigned int drivercount;
};

struct dm_device;
struct dm_driver
{
    char exists;
    char name[32];
    struct dm_class* dclass;
    void* data;
    unsigned int id;
    struct dm_device** devicelist;
    unsigned int devicecount;
};

struct dm_device
{
    char exists;
    char name[32];
    struct dm_driver* driver;
    void* data;
    unsigned int id;
};


void dm_init();
void dm_register_class(struct dm_class* dclass);
void dm_unregister_class(struct dm_class* dclass);
void dm_register_driver(struct dm_driver* driver);
void dm_unregister_driver(struct dm_driver* driver);
void dm_register_device(struct dm_device* device);
void dm_unregister_device(struct dm_device* device);


//INPUT/OUTPUT CLASS
struct dm_io_driver
{
    unsigned int(*read)(void* device,void* buffer,unsigned int bytes);
    unsigned int(*write)(void* device,void* buffer,unsigned int bytes);
    void* data;
};
void dm_io_register_driver(struct dm_io_driver* driver,char* name);
void dm_io_unregister_driver(struct dm_io_driver* driver);


//STORAGE CLASS
struct dm_storage_driver
{
    unsigned int(*read)(void* device,void* buffer,unsigned int bytes);
    unsigned int(*write)(void* device,void* buffer,unsigned int bytes);
    void* data;
};
void dm_storage_register_driver(struct dm_storage_driver* driver,char* name);
void dm_storage_unregister_driver(struct dm_storage_driver* driver);

#endif	/* DRIVER_MANAGER_H */

