#include "acpi.h"
#include "sys.h"
#include "libs/memory.h"
#include "cli.h"

struct acpi_rsdp
{
    char Signature[8];
    unsigned char Checksum;
    char OEMID[6];
    unsigned char Revision;
    unsigned int RsdtAddress;
};

struct acpi_rsdtheader
{
    char Signature[4];
    unsigned int Length;
    unsigned char Revision;
    unsigned char Checksum;
    char OEMID[6];
    char OEMTableID[8];
    unsigned int OEMRevision;
    unsigned int CreatorID;
    unsigned int CreatorRevision;
};

struct acpi_rsdt
{
    struct acpi_rsdtheader header;
};

char acpi_rsdpcorrect(struct acpi_rsdp* rsdp)
{
//Compare string with rsdp string
    if(rsdp->Signature[0]=='R'){if(rsdp->Signature[1]=='S'){
    if(rsdp->Signature[2]=='D'){if(rsdp->Signature[3]==' '){
    if(rsdp->Signature[4]=='P'){if(rsdp->Signature[5]=='T'){
    if(rsdp->Signature[6]=='R'){if(rsdp->Signature[7]==' '){
        return 1;
    } } } }
    } } } }

//Incorrect RSDP
    return 0;
}

struct acpi_rsdp* acpi_findrsdp()
{
//Variables
    unsigned int* addr;
    struct acpi_rsdp* rsdp=0;
    unsigned int ebda;

//Search for RSDP pointer
    for(addr=(unsigned int*)0x000E0000;(unsigned int)addr<0x00100000;addr+=0x10/sizeof(addr))
    {
        rsdp=(struct acpi_rsdp*)addr;
        if(acpi_rsdpcorrect(rsdp))
            return rsdp;
    }

    ebda=*((short*)0x40E)*0x10&0x000FFFFF;
    for(addr=(unsigned int*)ebda;(unsigned int)addr<ebda+1024;addr+=0x10/sizeof(addr))
    {
        rsdp=(struct acpi_rsdp*)addr;
        if(acpi_rsdpcorrect(rsdp))
            return rsdp;
    }

//Return Zero
    return 0;
}


void acpi_init()
{
//Find RSDP
    struct acpi_rsdp* rsdp=acpi_findrsdp();

//Print message if it doesnt exist
    if(rsdp==0){cli_puts("Error: RSDP not found.\n");for(;;);}

//Get RSDT address
    struct acpi_rsdt* rsdt=(struct acpi_rsdt*)rsdp->RsdtAddress;

//Find SDT pointers
    unsigned int *sdtptr=(unsigned int*)(rsdt+sizeof(struct acpi_rsdtheader));
    unsigned int sdtcount=(rsdt->header.Length-sizeof(struct acpi_rsdtheader))/4;

cli_putch('\n');
    cli_putu32(sdtcount,10);
cli_putch('\n');
}
