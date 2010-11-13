/* 
 * File:   vfs.h
 * Author: karl
 *
 * Created on 05 November 2010, 23:00
 */

#ifndef VFS_H
#define	VFS_H

struct vfs_filesystem_driver
{
    /*  Function pointers

     * open fs
     * close fs
     * list directory
     * open file
     * close file
     * read from file
     * write to file

    */
};

struct vfs_filesystem
{
    struct vfs_filesystem_driver* driver;
    void* data;
    char* mountpoint;
};

struct vfs_file
{
    struct vfs_filesystem* filesystem;
    void* data;
};


#endif	/* VFS_H */

