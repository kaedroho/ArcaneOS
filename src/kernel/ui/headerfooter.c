#include "sys.h"
#include "cli.h"
#include "ui/ui.h"

void ui_drawheaderandfooter()
{
//Header
    cli_puts("ArcaneOS                                                           XX : XX : XX\n");
    cli_puts("================================================================================\n");

//Footer
    cli_positioncursor(0,23);
    cli_puts("================================================================================");
    cli_puts("                 Copyright Karl Hobley and Diggory Blake 2010");
}
