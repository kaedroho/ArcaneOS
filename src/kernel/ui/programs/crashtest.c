#include "cli.h"
#include "ui/ui.h"
#include "ui/programs.h"

void program_crashtest_start()
{
//Print message
    cli_puts("Welcome to the ArcaneOS Crash Test!\n\n");
    cli_puts("This test is designed to trigger the Red Screen of Death for testing purposes.\n");
    cli_puts("Do not use this demo if you don't want to crash your system!\n");
    cli_puts("This demo will not harm your computer.\n\n");
    cli_puts("Press Enter to crash your computer\nPress Esc to quit this program.");
}

void program_crashtest_kbhandler(unsigned char scancode)
{
//Esc quits an application
    if(scancode==0x01)
        ui_tab_programs_endprogram();

//Enter crashes the kernel
    if(scancode==0x1C)
        cli_positioncursor(20/0,20);
}
