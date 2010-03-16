#ifndef _ACPI_H
#define _ACPI_H

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

struct acpi_dsdtheader
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

struct acpi_dsdt
{
    struct acpi_dsdtheader header;
};

struct acpi_main
{
    unsigned short SLP_TYPa;
    unsigned short SLP_TYPb;
    unsigned short SLP_EN;
    unsigned short SCI_EN;
};

extern struct acpi_rsdt* acpi_rsdtptr;
extern struct acpi_facp* acpi_facpptr;
extern struct acpi_dsdt* acpi_dsdtptr;

extern struct acpi_main acpi_main;

extern void acpi_init();
extern void acpi_enable();
#endif
