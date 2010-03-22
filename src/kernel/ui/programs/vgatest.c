#include "cli.h"
#include "ui/ui.h"
#include "ui/programs.h"
#include "video/video.h"
#include "video/drawer.h"

void program_vgatest_start()
{
//Print message
    cli_puts("Welcome to the ArcaneOS VGA Test!\n\n");
    cli_puts("Press enter to start test.\n");
    cli_puts("You can quit the program at any time by pressing Esc.\n");
}

void program_vgatest_runtest()
{
//Start VGA driver
    video_setdriver(video_vesa_getdriver(),0);

//Clear the screen and draw a box
    //video_cls();
    //draw_filledboxcolour(100,100,150,150,0x0F);
    //cli_puts(
}

void program_vgatest_kbhandler(unsigned char scancode)
{
//Esc quits an application
    if(scancode==0x01)
        ui_tab_programs_endprogram();

//Enter crashes the kernel
    if(scancode==0x1C)
        program_vgatest_runtest();
}
