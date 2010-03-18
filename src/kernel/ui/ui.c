#include "sys.h"
#include "cli.h"
#include "ui/ui.h"
#include "libs/rect.h"
#include "video/video.h"

struct ui_tab
{
    char* name;
    unsigned int namelength;
    void (*start)();
    void (*kbhandler)(unsigned char scancode);
};

void ui_clockhandler();

unsigned int ui_tabcount;
struct ui_tab ui_tablist[6];

struct Rect ui_CLIRect;

void ui_init()
{
//Setup rect
    ui_CLIRect.left=1;
    ui_CLIRect.right=video_getcollumns()-1;
    ui_CLIRect.top=3;
    ui_CLIRect.bottom=video_getrows()-5;

//Setup tabs
    ui_tabcount=6;
    ui_tablist[0].name="Programs";
    ui_tablist[0].namelength=sizeof("Programs");
    ui_tablist[0].start=ui_tab_programs_start;
    ui_tablist[0].kbhandler=ui_tab_programs_kbhandler;

    ui_tablist[1].name="Folders";
    ui_tablist[1].namelength=sizeof("Folders");
    ui_tablist[1].start=ui_tab_folders_start;
    ui_tablist[1].kbhandler=ui_tab_folders_kbhandler;

    ui_tablist[2].name="Settings";
    ui_tablist[2].namelength=sizeof("Settings");
    ui_tablist[2].start=ui_tab_settings_start;
    ui_tablist[2].kbhandler=ui_tab_settings_kbhandler;

    ui_tablist[3].name="Terminal";
    ui_tablist[3].namelength=sizeof("Terminal");
    ui_tablist[3].start=ui_tab_terminal_start;
    ui_tablist[3].kbhandler=ui_tab_terminal_kbhandler;

    ui_tablist[4].name="Power";
    ui_tablist[4].namelength=sizeof("Power");
    ui_tablist[4].start=ui_tab_power_start;
    ui_tablist[4].kbhandler=ui_tab_power_kbhandler;

    ui_tablist[5].name="About";
    ui_tablist[5].namelength=sizeof("About");
    ui_tablist[5].start=ui_tab_about_start;
    ui_tablist[5].kbhandler=ui_tab_about_kbhandler;

//Init tabs
    ui_tab_programs_init();
    ui_tab_folders_init();
    ui_tab_settings_init();
    ui_tab_terminal_init();
    ui_tab_power_init();
    ui_tab_about_init();
}

void ui_start()
{
//Clear screen
    cli_cls();cli_positioncursor(0,0);

//Draw header and footer
    ui_drawheaderandfooter();

//Position cursor
    cli_positioncursor(0,0);

//Set cli rect
    cli_setrect(&ui_CLIRect);

//Set tab
    ui_settab(0);

//Set clock handler
    rtc_setclockhandler(ui_clockhandler);
}

void ui_drawtabs(unsigned char tabid)
{
//Loop through tabs
    unsigned int i;
    for(i=0;i<ui_tabcount;i++)
    {
        cli_putch(' ');
        if(i==tabid)
        {
            cli_settextcolour(0xF0);
            cli_putch('[');
            cli_puts(ui_tablist[i].name);
            cli_putch(']');
            cli_settextcolour(0x0F);
        }else{
            cli_putch('[');
            cli_puts(ui_tablist[i].name);
            cli_putch(']');
        }
    }
}

void ui_settab(unsigned char tabid)
{
//Set keyboard handler
    kb_setkeyboardhandler(ui_tablist[tabid].kbhandler);

//Draw tab strip
    ui_CLIRect.top=2;
    cli_positioncursor(0,0);
    ui_drawtabs(tabid);
    ui_CLIRect.top=3;
    cli_positioncursor(0,0);

//Clear rect
    cli_clrect();

//Call start function
    ui_tablist[tabid].start();
}

void ui_clockhandler()
{
//Get time
    struct rtc_time* time=rtc_gettime();

//Draw time
    cli_setrect(0);
    cli_positioncursor(video_getcollumns()-13,0);
    cli_puts("            ");
    cli_positioncursor(video_getcollumns()-13,0);
    cli_putu32(time->hour,10);
    cli_positioncursor(video_getcollumns()-10,0);
    cli_putch(':');
    cli_positioncursor(video_getcollumns()-8,0);
    cli_putu32(time->minute,10);
    cli_positioncursor(video_getcollumns()-5,0);
    cli_putch(':');
    cli_positioncursor(video_getcollumns()-3,0);
    cli_putu32(time->seccond,10);

    cli_setrect(&ui_CLIRect);
    cli_positioncursor(0,0);


}
