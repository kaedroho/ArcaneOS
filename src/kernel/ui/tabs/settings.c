#include "sys.h"
#include "ui/ui.h"
#include "cli.h"

void ui_tab_settings_init()
{

}

void ui_tab_settings_start()
{
    cli_puts("Settings");
}

void ui_tab_settings_kbhandler(unsigned char scancode)
{
//Change tab with left and right keys
    if(scancode==0x4B)
        ui_settab(1);
    if(scancode==0x4D)
        ui_settab(3);
}
