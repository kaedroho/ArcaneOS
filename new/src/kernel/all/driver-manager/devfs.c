#include <vfs.h>

char devfs_started=0;


void* devfs_openfs(void* data);
void devfs_closefs(void* fsdata);
void* devfs_openfile(void* fsdata,char* name);
void devfs_closefile(void* filedata);
unsigned int devfs_readfile(void* file,void* buffer,unsigned int bytes);
unsigned int devfs_writefile(void* file,void* buffer,unsigned int bytes);

struct dm_fs_driver devfs_fsdriver={
    .openfs=devfs_openfs,
    .closefs=devfs_closefs,
    .openfile=devfs_openfile,
    .closefile=devfs_closefile,
    .readfile=devfs_readfile,
    .writefile=0
};


void* devfs_openfs(void* data)     //Callback function
{
    if(devfs_started)
        return 0;

    devfs_started=1;
    return (void*)1;
}

void devfs_closefs(void* fsdata)     //Callback function
{
    if((int)fsdata==1 && devfs_started==1)
        devfs_started=0;

}


void* devfs_openfile(void* fsdata,char* name)     //Callback function
{
    return 0;
}

void devfs_closefile(void* filedata)     //Callback function
{

}


unsigned int devfs_readfile(void* file,void* buffer,unsigned int bytes)     //Callback function
{
    return 0;
}

unsigned int devfs_writefile(void* file,void* buffer,unsigned int bytes)     //Callback function
{
    return 0;
}