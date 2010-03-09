#include "sys.h"
#include "ui/ui.h"
#include "cli.h"

void ui_tab_about_init()
{

}

void ui_tab_about_start()
{
    cli_puts("\nArcaneOS version 0.01\n\nProgrammers:\nKarl Hobley\nDiggory Blake");
}

void ui_tab_about_kbhandler(unsigned char scancode)
{
//Change tab with left and right keys
    if(scancode==0x4B)
        ui_settab(3);
}
