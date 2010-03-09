#include "sys.h"
#include "ui/ui.h"
#include "cli.h"

void ui_tab_folders_init()
{

}

void ui_tab_folders_start()
{
    cli_puts("Folders");
}

void ui_tab_folders_kbhandler(unsigned char scancode)
{
//Change tab with left and right keys
    if(scancode==0x4B)
        ui_settab(0);
    if(scancode==0x4D)
        ui_settab(2);
}
