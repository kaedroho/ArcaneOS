/* 
 * File:   vfs.h
 * Author: karl
 *
 * Created on 05 November 2010, 23:00
 */

#ifndef VFS_H
#define	VFS_H

#include <dm.h>

struct vfs_filesystem_driver
{
    void*(*openfs)(void* data);
    void(*closefs)(void* fsdata);
    void*(*openfile)(void* fsdata,char* name);
    void(*closefile)(void* filedata) ;
    unsigned int(*readfile)(void* file,void* buffer,unsigned int bytes);
    unsigned int(*writefile)(void* file,void* buffer,unsigned int bytes);

    void* data;
};

struct vfs_filesystem
{
    char inuse; //(bool)
    struct vfs_filesystem_driver* driver;
    void* data;
    char* mountpoint;
    unsigned int mountpointcount;
    struct vfs_filesystem* mountpoints;
};

struct vfs_file
{
    struct vfs_filesystem* filesystem;
    void* data;
};


void vfs_init();
struct vfs_filesystem* vfs_open_filesystem(struct vfs_filesystem_driver* driver,void* data,char* mountpoint);
void vfs_close_filesystem(struct vfs_filesystem* filesystem);
struct vfs_file* vfs_open_file(struct vfs_filesystem* filesystem,struct vfs_file* file,char* name);
void vfs_close_file(struct vfs_file* file);
unsigned int vfs_readfile(struct vfs_file* file,void* buffer,unsigned int bytes);


#endif	/* VFS_H */

