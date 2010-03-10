#include "sys.h"
#include "cli.h"
#include "ui/ui.h"

//lol
void ui_drawheaderandfooter()
{
//Header
    cli_puts("ArcaneOS\n");
    cli_puts("================================================================================\n");

//Footer
    cli_positioncursor(0,23);
    cli_puts("================================================================================");
    cli_puts("                            Copyright Karl Hobley 2010");
}
