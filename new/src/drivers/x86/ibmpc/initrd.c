#include <vfs.h>
#include <string.h>

struct initrd_file;
struct initrd_filesystem
{
    void* ptr;
    unsigned long filecount;
    struct initrd_file* filelist;
};

struct initrd_file
{
    char* name;
    struct initrd_filesystem* filesystem;
    unsigned long offset;
    char isopen; //(bool)
};


struct vfs_filesystem_driver initrd_fsdriver;

void initrd_init()
{

}


void* initrd_openfs(void* data)     //Callback function
{
    return 0;
}

void initrd_closefs(void* fsdata)     //Callback function
{

}


void* initrd_openfile(void* fsdata,char* name)     //Callback function
{
//Get filesystem data
    struct initrd_filesystem* fs=(struct initrd_filesystem*)fsdata;

//Search for and open the file
    unsigned long i=0;
    for(i=0;i<fs->filecount;i++){
        if(strcmp(fs->filelist[i].name,name)){
            if(fs->filelist[i].isopen==0){
                fs->filelist[i].isopen=1;
                return (void*)&fs->filelist[i];
            }else{
                return 0; //ALREADY OPEN
            }
        }
    }
    return 0; //NOT FOUND
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