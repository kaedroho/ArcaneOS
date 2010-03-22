#include "boot.h"

#include "cli.h"
#include "ui/ui.h"
#include "ui/programs.h"
#include "syscall.h"
#include "floppy.h"
#include "mt.h"

void run_floppy_test()
{
    unsigned char buffer[32];
    unsigned int error;
    int i;

    cli_puts("Welcome to the ArcaneOS floppy test program!\n\n");
    if (!floppy_drive_exists())
    {
        cli_puts("No floppy drive detected!");
        for(;;) syscall_sleep(10000); // Can't delete threads yet :(
    }

    cli_puts("Reading 32 bytes from the floppy...\n");
    syscall_floppy_read(0,0,0,0,32,buffer,&error);

    while (error == floppy_error_queueing)
        syscall_sleep(50);

    if (error)
    {
        cli_puts("Floppy read failed with error code ");
        cli_putu32(error,10);
        cli_puts("!\n");
        for(;;) syscall_sleep(10000); // Can't delete threads yet :(
    }

    cli_puts("Floppy read succeeded!\n\n");
    cli_puts("Data:\n");
    for (i = 0; i < 32; i++)
    {
        cli_putu32(buffer[i],16);
        if ((i-1) % 8)
            cli_puts("   ");
        else
            cli_puts("\n");
    }

    for(;;) syscall_sleep(10000); // Can't delete threads yet :(
}

void program_floppytest_start()
{
    mt_create_thread(mt_kernel_process,run_floppy_test,1);
}

void program_floppytest_kbhandler(unsigned char scancode)
{
//Esc quits an application
    if(scancode==0x01)
        ui_tab_programs_endprogram();
}
