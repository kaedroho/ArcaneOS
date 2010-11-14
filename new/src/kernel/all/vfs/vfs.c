#include <vfs.h>
#include <string.h>

const unsigned int vfs_mountpointcount=256;
struct vfs_filesystem vfs_mountpoints[256]; //Temporarily using static memory to store mount point array


void vfs_init()
{
    memset((unsigned char*)vfs_mountpoints,0,256*sizeof(struct vfs_filesystem));
}


struct vfs_filesystem* vfs_open_filesystem(struct vfs_filesystem_driver* driver,void* data,char* mountpoint)
{
    void* fsdata=0;
    if(driver->openfs!=0)
        fsdata=driver->openfs(data);

    if(fsdata==0)
        return 0;

    unsigned int i;
    char foundmp=0;
    for(i=0;i<vfs_mountpointcount;i++){
        if(vfs_mountpoints[i].inuse==0){
            vfs_mountpoints[i].inuse=1;
            vfs_mountpoints[i].data=fsdata;
            vfs_mountpoints[i].driver=driver;
            vfs_mountpoints[i].mountpoint=mountpoint;
            foundmp=1;
            break;
        }
    }

    if(foundmp==1){
        return &vfs_mountpoints[i];
    }else{
        if(driver->closefs!=0)
            driver->closefs(fsdata);
        return 0;
    }
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