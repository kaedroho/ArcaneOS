#ifndef _PROGRAMS_H
#define _PROGRAMS_H

//General functions
extern void ui_tab_programs_endprogram();

//Crashtest functions
extern void program_crashtest_start();
extern void program_crashtest_kbhandler(unsigned char scancode);

//Debug info functions
extern void program_debuginfo_start();
extern void program_debuginfo_kbhandler(unsigned char scancode);

//VGA test functions
extern void program_vgatest_start();
extern void program_vgatest_kbhandler(unsigned char scancode);

//CPUID Info functions
extern void program_cpuidinfo_start();
extern void program_cpuidinfo_kbhandler(unsigned char scancode);

//CPUID Info functions
extern void program_floppytest_start();
extern void program_floppytest_kbhandler(unsigned char scancode);
#endif
