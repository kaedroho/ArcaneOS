#include "sys.h"
#include "ui/ui.h"
#include "cli.h"
#include "video/video.h"

struct ui_tab_power_item
{
   char *name;
    unsigned int namelength;
    void (*run)();
};

unsigned int ui_tab_power_itemcount;
unsigned int ui_tab_power_currentselecteditem;
struct ui_tab_power_item ui_tab_power_itemlist[2];

void ui_tab_power_shutdown();
void ui_tab_power_restart();

void ui_tab_power_init()
{
//Zero variables
    ui_tab_power_itemcount=2;
    ui_tab_power_currentselecteditem=0;

//Add shutdown
    ui_tab_power_itemlist[0].name="Shutdown";
    ui_tab_power_itemlist[0].namelength=sizeof("Shutdown");
    ui_tab_power_itemlist[0].run=ui_tab_power_shutdown;

//Add restart
    ui_tab_power_itemlist[1].name="Restart";
    ui_tab_power_itemlist[1].namelength=sizeof("Restart");
    ui_tab_power_itemlist[1].run=ui_tab_power_restart;
}

void ui_tab_power_drawlist()
{
//Position cursor at beginning
    cli_positioncursor(0,0);

//Loop through items
    unsigned int i;
    for(i=0;i<ui_tab_power_itemcount;i++)
    {
        cli_putch('\n');
        if(i==ui_tab_power_currentselecteditem)
        {
            cli_settextcolour(0xF0);
            cli_putch('[');
            cli_puts(ui_tab_power_itemlist[i].name);
            cli_putch(']');
            cli_settextcolour(0x0F);
        }else{
            cli_putch('[');
            cli_puts(ui_tab_power_itemlist[i].name);
            cli_putch(']');
        }
    }
}

void ui_tab_power_start()
{
//Draw item list
    ui_tab_power_drawlist();
}

void ui_tab_power_kbhandler(unsigned char scancode)
{
//Change tab with left and right keys
    if(scancode==0x4B)
        ui_settab(3);
    if(scancode==0x4D)
        ui_settab(5);

//Select item with up and down keys
    if(scancode==0x50)
    {
        if(ui_tab_power_currentselecteditem==ui_tab_power_itemcount-1){
            ui_tab_power_currentselecteditem=0;
        }else{
            ui_tab_power_currentselecteditem++;
        }
        ui_tab_power_drawlist();
    }
    if(scancode==0x48)
    {
        if(ui_tab_power_currentselecteditem==0){
            ui_tab_power_currentselecteditem=ui_tab_power_itemcount-1;
        }else{
            ui_tab_power_currentselecteditem--;
        }
        ui_tab_power_drawlist();
    }

//Run item with enter key
    if(scancode==0x1C)
        ui_tab_power_itemlist[ui_tab_power_currentselecteditem].run();
}


void ui_tab_power_shutdown()
{
    power_shutdown();
}

void ui_tab_power_restart()
{
    power_reboot();
}
