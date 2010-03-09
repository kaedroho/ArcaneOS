#include "sys.h"
#include "ui/ui.h"
#include "cli.h"

void ui_tab_terminal_init()
{

}

void ui_tab_terminal_start()
{
    cli_puts("Terminal");
}

void ui_tab_terminal_kbhandler(unsigned char scancode)
{
//Change tab with left and right keys
    if(scancode==0x4B)
        ui_settab(2);
    if(scancode==0x4D)
        ui_settab(4);
}
