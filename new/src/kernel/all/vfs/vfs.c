#include <vfs.h>

void vfs_init()
{

}

struct vfs_filesystem* vfs_open_filesystem(struct vfs_filesystem_driver* driver,void* data,char* mountpoint)
{
    return 0;
}

void vfs_close_filesystem(struct vfs_filesystem* filesystem)
{

}



struct vfs_file* vfs_open_file(char* name)
{
    return 0;
}

void vfs_close_file(struct vfs_file* file)
{

}