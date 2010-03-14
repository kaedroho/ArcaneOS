#include "sys.h"
#include "cli.h"
#include "ui/ui.h"
#include "video/video.h"

void ui_drawheaderandfooter()
{
//Variables
    unsigned int i;
//Header
    cli_puts("ArcaneOS");

    for(i=0;i<video_getcollumns()-21;i++)
        cli_putch(' ');

    cli_puts("XX : XX : XX\n");

    for(i=0;i<video_getcollumns();i++)
        cli_putch('=');

//Footer
    cli_positioncursor(0,video_getrows()-2);

    for(i=0;i<video_getcollumns();i++)
        cli_putch('=');

    for(i=0;i<video_getcollumns()/2-22;i++)
        cli_putch(' ');
    cli_puts("Copyright Karl Hobley and Diggory Blake 2010");
}
