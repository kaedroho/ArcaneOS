#include <stdio.h>
#include <dirent.h>
#include <string.h>

struct initrd_header
{
    unsigned long Magic;
    unsigned char FileCount;
};

struct initrd_file
{
    char name[32];
    unsigned long offset;
    unsigned long size;
};



int main(int argc, char** argv)
{
//Variables
    char* OutFile="init.rd";
    char* Input="files";

//Process commandline
    int i;
    for(i=1;i<argc;i++){
        if(strcmp(argv[i],"--help")==0){
            printf("ArcaneOS initrd builder Version 0.01. Created by Karl Hobley\n"
                    "build-initrd [-o OUTPUTFILE] [-i INPUTDIRECTORY]\n"
                    "By default the output file is 'init.rd' and input directory is 'files'\n");
            return 0;
        }else if(strcmp(argv[i],"-o")==0){
            OutFile=argv[i+1];
            i++;
        }else if(strcmp(argv[i],"-i")==0){
            Input=argv[i+1];
            i++;
        }
    }

//Make main header
    struct initrd_header Header;
    Header.Magic=0xACAED123;

//Open input directory
    DIR* InputDIR=opendir(Input);
    if(InputDIR==0){
        printf("ERROR: input directory doesn't exist.\n");
        return -1;
    }
    chdir(Input);

//File variables
    struct initrd_file FileHeader[256];
    Header.FileCount=0;
    unsigned long CurrentOffset=0;

//Find files
    struct dirent *ENT;
    while(ENT=readdir(InputDIR)){
        if(ENT->d_type==DT_REG){
            printf("Found File: %s\n",ENT->d_name);
            strncpy(FileHeader[Header.FileCount].name,ENT->d_name,32);
            FileHeader[Header.FileCount].offset=CurrentOffset;
            FILE* File=fopen(ENT->d_name,"rb");
            fseek(File,0,SEEK_END);
            FileHeader[Header.FileCount].size=ftell(File);
            CurrentOffset+=FileHeader[Header.FileCount].size;
            fclose(File);
            Header.FileCount++;
        }
    }
    closedir(InputDIR);
    chdir("..");

//Make output file and write headers
    FILE* OutputFile=fopen(OutFile,"wb");
    chdir(Input);
    fwrite(&Header,sizeof(struct initrd_header),1,OutputFile);
    fwrite(&FileHeader,sizeof(struct initrd_file),Header.FileCount,OutputFile);

//Open input directory
    chdir(Input);

//Write files
    for(i=0;i<Header.FileCount;i++){
        FILE* File=fopen(FileHeader[i].name,"rb");
        int chr=getc(File);
        while(chr!=EOF){
            fputc(chr,OutputFile);
            chr=getc(File);
        }
        fclose(File);
    }

//Close output file and exit
    fclose(OutputFile);
    printf("Successfully created %s\n",OutFile);
    return 0;
}

