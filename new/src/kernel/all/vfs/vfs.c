#include <vfs.h>
#include <string.h>

const unsigned int vfs_mountpointcount=256;
struct vfs_filesystem vfs_mountpoints[256];
struct vfs_filesystem vfs_rootfs;


void vfs_init()
{
    memset((unsigned char*)vfs_mountpoints,0,256*sizeof(struct vfs_filesystem));
    vfs_rootfs.mountpointcount=256;
    vfs_rootfs.mountpoints=vfs_mountpoints;
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
            vfs_mountpoints[i].mountpointcount=0;
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
    filesystem->inuse=0;
    if(filesystem->driver->closefs!=0)
        filesystem->driver->closefs(filesystem->data);
}


#include <console.h>
struct vfs_file* _vfs_open_file(struct vfs_filesystem* filesystem,struct vfs_file* file,char* name)
{
//Variables
    void* fdata=0;

//Call driver to open file
    if(filesystem->driver->openfile!=0)
        fdata=filesystem->driver->openfile(filesystem->data,name);

//If it succeeded, return a file structure. If not, return 0
    if(fdata!=0){
        file->data=fdata;
        file->filesystem=filesystem;
        return file;
    }else{
        return 0;
    }
}
struct vfs_file* vfs_open_file(struct vfs_filesystem* filesystem,struct vfs_file* file,char* name)
{
    if(filesystem->mountpointcount==0){
    //Attempt to load the file in this filesystem
        return _vfs_open_file(filesystem,file,name);
    }else{
    //Loop through mountpoints to check if the file is in one
        unsigned int i=0;
        for(i=0;i<filesystem->mountpointcount;i++){
        //Check if this mountpoint contains this file
            unsigned int mpsize=strcmpbegin(filesystem->mountpoints[i].mountpoint,name);
            if(mpsize>0){
            //It does, call this function for this mountpoint
                return vfs_open_file(&filesystem->mountpoints[i],file,(char*)((void*)name)+mpsize+1);
            }
        }

    //Attempt to load the file in this filesystem
        return _vfs_open_file(filesystem,file,name);

    }
    return 0;
}

void vfs_close_file(struct vfs_file* file)
{

}

unsigned int vfs_readfile(struct vfs_file* file,void* buffer,unsigned int bytes)
{
    console_putu32_protected(bytes,10);
    if(file->filesystem->driver->readfile!=0)
        return file->filesystem->driver->readfile(file->data,buffer,bytes);
    else
        return 0;
}