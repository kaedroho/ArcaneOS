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
    struct dm_class dclass;
};

struct dm_device
{
    char name[32];
    struct dm_driver driver;
};


void dm_init();
void dm_register_class(struct dm_class* dclass);
void dm_unregister_class(struct dm_class* dclass);;
void dm_register_driver(struct dm_driver* driver);
void dm_unregister_driver(struct dm_driver* driver);
void dm_register_device(struct dm_device* device);
void dm_unregister_device(struct dm_device* device);

#endif	/* DRIVER_MANAGER_H */

