#ifndef _UI_H
#define _UI_H

//UI main funcitons
extern void ui_init();
extern void ui_start();
extern void ui_settab(unsigned char tabid);

//UI internal functions
extern void ui_drawheaderandfooter();

//UI tabs functions
extern void ui_tab_programs_init();
extern void ui_tab_programs_start();
extern void ui_tab_programs_kbhandler(unsigned char scancode);
extern void ui_tab_folders_init();
extern void ui_tab_folders_start();
extern void ui_tab_folders_kbhandler(unsigned char scancode);
extern void ui_tab_settings_init();
extern void ui_tab_settings_start();
extern void ui_tab_settings_kbhandler(unsigned char scancode);
extern void ui_tab_terminal_init();
extern void ui_tab_terminal_start();
extern void ui_tab_terminal_kbhandler(unsigned char scancode);
extern void ui_tab_power_init();
extern void ui_tab_power_start();
extern void ui_tab_power_kbhandler(unsigned char scancode);
extern void ui_tab_about_init();
extern void ui_tab_about_start();
extern void ui_tab_about_kbhandler(unsigned char scancode);

#endif
