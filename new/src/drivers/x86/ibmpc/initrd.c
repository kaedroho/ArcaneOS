#include <vfs.h>
#include <string.h>

struct initrd_filesystem
{
    unsigned long magic;
    unsigned char filecount;
    char isopen; //(bool)
};

struct initrd_file
{
    char name[32];
    void* initrd_base_ptr;
    unsigned int offset;
    unsigned int size;
    char isopen; //(bool)
    unsigned int currentpos;
};


struct vfs_filesystem_driver initrd_fsdriver;


void* initrd_openfs(void* data)     //Callback function
{
    if(((struct initrd_filesystem*)data)->magic==0xACAED123){
        if(((struct initrd_filesystem*)data)->isopen){
            return 0;   //Already open
        }else{
            ((struct initrd_filesystem*)data)->isopen=1;

        //Setup files
            void* filestart=data+sizeof(struct initrd_filesystem);
            unsigned int i=0;
            for(i=0;i<((struct initrd_filesystem*)data)->filecount;i++){
                struct initrd_file* File=(struct initrd_file*)filestart+i;
                File->initrd_base_ptr=data;
            }

            return data;
        }
    }else{
        return 0;   //Invalid image
    }
}

void initrd_closefs(void* fsdata)     //Callback function
{
    if(((struct initrd_filesystem*)fsdata)->magic==0xACAED123)
        ((struct initrd_filesystem*)fsdata)->isopen=0;
}


void* initrd_openfile(void* fsdata,char* name)     //Callback function
{
    struct initrd_filesystem* FS=(struct initrd_filesystem*)fsdata;
    void* filestart=fsdata+sizeof(struct initrd_filesystem);
    if(FS->magic==0xACAED123 && FS->isopen){
        unsigned char i=0;
        for(i=0;i<FS->filecount;i++){
            struct initrd_file* File=(struct initrd_file*)filestart+i;
            if(strcmp(File->name,name)){
                if(File->isopen==0){
                    File->isopen=1;
                    File->currentpos=0;
                    return (void*)File; //Success
                }else{
                    return 0;   //Already open
                }
            }
        }
        return 0;   //Not found
    }else{
        return 0;   //Invalid filesystem
    }
}

void initrd_closefile(void* filedata)     //Callback function
{
//Get file data
    struct initrd_file* file=(struct initrd_file*)filedata;

//Close the file
    file->isopen=0;
}

unsigned int initrd_readfile(void* file,void* buffer,unsigned int bytes)
{
    if(bytes==0)
        return 0;
    struct initrd_file* File=(struct initrd_file*)file;
    struct initrd_filesystem* FS=(struct initrd_filesystem*)File->initrd_base_ptr;
    if(FS->magic==0xACAED123 && FS->isopen){
        if(File->isopen){
            void* data=File->initrd_base_ptr+sizeof(struct initrd_filesystem)+FS->filecount*sizeof(struct initrd_file)+File->offset;
            unsigned int currentbyte=0;
            while(!(File->currentpos>=File->size) && !(currentbyte>=bytes)){
                ((char*)buffer)[currentbyte]=*(char*)(data+File->currentpos);
                File->currentpos++;
                currentbyte++;
            }
            ((char*)buffer)[currentbyte]=0;
            return currentbyte;
        }else{
            *(char*)buffer=0;
            return 0; //File not open
        }
    }else{
        *(char*)buffer=0;
        return 0;   //Invalid filesystem
    }
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