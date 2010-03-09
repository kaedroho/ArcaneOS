#include "cli.h"
#include "ui/ui.h"
#include "ui/programs.h"

unsigned char program_crashtest_exit;
void program_crashtest_main()
{
//Zero exit variable
    program_crashtest_exit=0;

//Print message


//Endless loop
    while(program_crashtest_exit==0);
}

void program_crashtest_kbhandler(unsigned char scancode)
{
//Esc quits an application
    if(scancode==0x73)
        program_crashtest_exit=1;
}
