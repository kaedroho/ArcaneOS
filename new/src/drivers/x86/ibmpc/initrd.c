#include <vfs.h>
#include <string.h>

struct initrd_filesystem
{
    unsigned long Magic;
    unsigned char FileCount;
    char isopen; //(bool)
};

struct initrd_file
{
    char name[32];
    void* initrd_base_ptr;
    unsigned long offset;
    unsigned long size;
    char isopen; //(bool)
};


struct vfs_filesystem_driver initrd_fsdriver;


void* initrd_openfs(void* data)     //Callback function
{
    return 0;
}

void initrd_closefs(void* fsdata)     //Callback function
{

}


void* initrd_openfile(void* fsdata,char* name)     //Callback function
{
    return 0;
}

void initrd_closefile(void* filedata)     //Callback function
{
//Get file data
    struct initrd_file* file=(struct initrd_file*)filedata;

//Close the file
    file->isopen=0;
}

void initrd_readfile(void* file,void* buffer,int bytes)
{

}


void initrd_init()
{
    initrd_fsdriver.openfs=initrd_openfs;
    initrd_fsdriver.closefs=initrd_closefs;
    initrd_fsdriver.openfile=initrd_openfile;
    initrd_fsdriver.closefile=initrd_closefile;
    initrd_fsdriver.readfile=initrd_readfile;
    initrd_fsdriver.writefile=0;
}