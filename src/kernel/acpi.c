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

struct acpi_facp
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
    unsigned int FirmwareCtrlPtr;
    unsigned int DSDTPtr;
    unsigned char INT_MODEL;
    char reserved1;
    unsigned short SCI_INT;
    unsigned int SMI_CMD;
    unsigned char ACPI_ENABLE;
    unsigned char ACPI_DISABLE;
    unsigned char S4BIOS_REQ;
    char reserved2;
    unsigned int PM1a_EVT_BLK;
    unsigned int PM1b_EVT_BLK;
    unsigned int PM1a_CNT_BLK;
    unsigned int PM1b_CNT_BLK;
    unsigned int PM2_CNT_BLK;
    unsigned int PM_TMR_BLK;
    unsigned int GPE0_BLK;
    unsigned int GPE1_BLK;
    unsigned char PM1_EVT_LEN;
    unsigned char PM1_CNT_LEN;
    unsigned char PM2_CNT_LEN;
    unsigned char PM_TM_LEN;
    unsigned char GPE0_BLK_LEN;
    unsigned char GPE1_BLK_LEN;
    unsigned char GPE0_BASE;
    char reserved3;
    unsigned short P_LVL2_LAT;
    unsigned short P_LVL3_LAT;
    unsigned short FLUSH_SIZE;
    unsigned short FLUSH_STRIDE;
    unsigned char DUTY_OFFSET;
    unsigned char DUTY_WIDTH;
    unsigned char DAY_ALRM;
    unsigned char MON_ALRM;
    unsigned char CENTURY;
    char reserved4[3];
    unsigned int Flags;
};

struct acpi_rsdt* acpi_rsdtptr;
struct acpi_facp* acpi_facpptr;

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
    acpi_rsdtptr=(struct acpi_rsdt*)rsdp->RsdtAddress;

//Find SDT pointers
    unsigned int sdtptr=(unsigned int)(rsdp->RsdtAddress+sizeof(struct acpi_rsdtheader));
    unsigned int sdtcount=(acpi_rsdtptr->header.Length-sizeof(struct acpi_rsdtheader))/4;

    unsigned int i;
    acpi_facpptr=0;
    for(i=0;i<sdtcount;i++)
    {
        if(memcmp(*(char**)(sdtptr+i*4),"FACP",4))
            acpi_facpptr=*(struct acpi_facp**)(sdtptr+i*4);
    }

//Check if facp exists
    if(acpi_facpptr==0){cli_puts("Error: FACP not found.\n");for(;;);}

    //for(;;);
}


void acpi_enable()
{

}
