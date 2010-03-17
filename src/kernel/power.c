#include "acpi.h"
#include "sys.h"

void power_init()
{

}

void power_shutdown()
{
//Send shutdown command
    outw(acpi_facpptr->PM1a_CNT_BLK,acpi_main.SLP_TYPa|acpi_main.SLP_EN);
    if(acpi_facpptr->PM1b_CNT_BLK!=0)
        outw(acpi_facpptr->PM1b_CNT_BLK,acpi_main.SLP_TYPb|acpi_main.SLP_EN);
}

void power_reboot()
{
//Send reset command to keyboard controller
    outb(0x64,0xFE);
}
