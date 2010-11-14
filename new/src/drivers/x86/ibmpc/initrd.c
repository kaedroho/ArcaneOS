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
    if(((struct initrd_filesystem*)data)->Magic==0xACAED123){
        if(((struct initrd_filesystem*)data)->isopen){
            return 0;   //Already open
        }else{
            ((struct initrd_filesystem*)data)->isopen=1;

            //Setup files

            return data;
        }
    }else{
        return 0;   //Invalid image
    }
}

void initrd_closefs(void* fsdata)     //Callback function
{
    if(((struct initrd_filesystem*)fsdata)->Magic==0xACAED123)
        ((struct initrd_filesystem*)fsdata)->isopen=0;
}


void* initrd_openfile(void* fsdata,char* name)     //Callback function
{
    struct initrd_filesystem* FS=(struct initrd_filesystem*)fsdata;
    if(FS->Magic==0xACAED123 && FS->isopen){
        unsigned char i=0;
        for(i=0;i<FS->FileCount;i++){
            struct initrd_file* File=(struct initrd_file*)fsdata+sizeof(struct initrd_filesystem)+sizeof(struct initrd_file)*i;
            if(strcmp(File->name,name)){

            }
        }
    }else{
        return 0;
    }
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