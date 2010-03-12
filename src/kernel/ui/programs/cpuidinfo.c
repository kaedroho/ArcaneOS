#include "cli.h"
#include "ui/ui.h"
#include "ui/programs.h"
#include "sys.h"

void program_cpuidinfo_start()
{
//Print message
    cli_puts("Welcome to the ArcaneOS CPUID info program!\n\n");
    cli_puts("This program shows all the information returned by CPUID\n");
    cli_puts("Press Esc to quit the program\n\n\n");

//Put main CPU ID info
    struct cpuid_struct* cpuidstruct=cpuid_getstruct();
    cli_puts("Vendor: ");cli_puts(cpuidstruct->vendor);cli_putch('\n');
}

void program_cpuidinfo_kbhandler(unsigned char scancode)
{
//Esc quits an application
    if(scancode==0x01)
        ui_tab_programs_endprogram();
}
