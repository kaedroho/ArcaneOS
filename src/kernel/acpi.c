#include "acpi.h"
#include "sys.h"
#include "libs/memory.h"
#include "cli.h"

struct acpi_rsdt* acpi_rsdtptr;
struct acpi_facp* acpi_facpptr;
struct acpi_dsdt* acpi_dsdtptr;

struct acpi_main acpi_main;

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

//Get DSDT
    acpi_dsdtptr=(struct acpi_dsdt*)acpi_facpptr->DSDTPtr;

//Find \_S5
    unsigned int DSDTLength=acpi_dsdtptr->header.Length-36;
    char *S5Ptr=(char*)acpi_dsdtptr+36;
    while(0<DSDTLength)
    {
    //Check if this is the S5 Pointer
        if(memcmp(S5Ptr,"_S5_",4))
            break;

    //Next byte
        S5Ptr++;
    }

//Check if \_S5 was found and parse it
    if(DSDTLength>0)
    {
        if((*(S5Ptr-1)==0x08 || (*(S5Ptr-2)==0x08 && *(S5Ptr-1)=='\\')) && *(S5Ptr+4)==0x12)
        {
        //Calculate pkglength size
            S5Ptr+=5;
            S5Ptr+=((*S5Ptr&0xC0)>>6)+2;

        //Get SLP_TYPa
            if(*S5Ptr==0x0A)
                S5Ptr++;
            acpi_main.SLP_TYPa=*(S5Ptr)<<10;

        //Next byte
            S5Ptr++;

        //Get SLP_TYPb
            if(*S5Ptr==0x0A)
                S5Ptr++;
            acpi_main.SLP_TYPb=*(S5Ptr)<<10;
        }
    }

//Set some other variables
    acpi_main.SLP_EN=1<<13;
    acpi_main.SCI_EN=1;
}

char acpi_checkenabled()
{
    return inw(acpi_facpptr->PM1a_CNT_BLK)&acpi_main.SCI_EN;
}

void acpi_enable()
{
//Check that ACPI isnt already enabled
    if(acpi_checkenabled()==0)
    {
    //Check if ACPI can be enabled
        if(acpi_facpptr->SMI_CMD!=0&&acpi_facpptr->ACPI_ENABLE!=0)
        {
        //Enable ACPI
            outb(acpi_facpptr->SMI_CMD,acpi_facpptr->ACPI_ENABLE);

        //Wait for 3 secconds
            unsigned int starttime=timer_getticks();
            while(starttime+timer_getticksperseccond()*3>timer_getticks())
            {
                if((timer_getticks()-starttime)%50)
                    if(acpi_checkenabled())
                        break;
            }

        //Check if ACPI has been enabled
            if (acpi_checkenabled()==0){cli_puts("Error: ACPI cannot be enabled.\n");for(;;);}
        }
    }
}
