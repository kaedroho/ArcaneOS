#include "sys.h"
#include "ui/ui.h"
#include "cli.h"
#include "ui/programs.h"

struct ui_tab_program_program
{
   char *name;
    unsigned int namelength;
    void (*main)();
    void (*kbhandler)(unsigned char scancode);
};

unsigned int ui_tab_program_programcount;
unsigned int ui_tab_program_currentselectedprogram;
struct ui_tab_program_program ui_tab_program_programlist[1];

void ui_tab_programs_init()
{
//Zero variables
    ui_tab_program_programcount=1;
    ui_tab_program_currentselectedprogram=0;

//Add crashtest
    ui_tab_program_programlist[0].name="Crashtest";
    ui_tab_program_programlist[0].namelength=sizeof("Crashtest");
    ui_tab_program_programlist[0].main=program_crashtest_main;
    ui_tab_program_programlist[0].kbhandler=program_crashtest_kbhandler;
}

void ui_tab_programs_drawlist()
{
//Position cursor at beginning
    cli_positioncursor(0,0);

}

void ui_tab_programs_start()
{
//Draw program list
    ui_tab_programs_drawlist();
}

void ui_tab_programs_runprogram(unsigned int programid)
{
//Clear the screen
    cli_cls();
    cli_setrect(0);

//Set keyboard handler
    kb_setkeyboardhandler(ui_tab_program_programlist[programid].kbhandler);

//Call main function
    ui_tab_program_programlist[programid].main();

//Restart UI
    ui_start();
}

void ui_tab_programs_kbhandler(unsigned char scancode)
{
//Change tab with left and right keys
    if(scancode==0x4D)
        ui_settab(1);

//Select program with up and down keys
    if(scancode==0x48)
    {
        if(ui_tab_program_currentselectedprogram==ui_tab_program_programcount){
            ui_tab_program_currentselectedprogram=0;
        }else{
            ui_tab_program_currentselectedprogram++;
        }
        ui_tab_programs_drawlist();
    }
    if(scancode==0x50)
    {
        if(ui_tab_program_currentselectedprogram==0){
            ui_tab_program_currentselectedprogram=ui_tab_program_programcount-1;
        }else{
            ui_tab_program_currentselectedprogram--;
        }
        ui_tab_programs_drawlist();
    }

//Run program with enter key
    if(scancode==0x1C)
    {
    //Run program
        ui_tab_programs_runprogram(ui_tab_program_currentselectedprogram);
    }
}
