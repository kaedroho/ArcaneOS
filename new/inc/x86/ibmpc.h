#ifndef IBMPC_H
#define	IBMPC_H

//MAIN

void ibmpc_init();


//KEYBOARD

struct KB_Layout {
    char defaultMapping[128];
    char shiftedMapping[128];
};

void kb_init();
void kb_setlayout(struct KB_Layout* layout);
void kb_hitbutton(unsigned char scancode);
void kb_setkeyboardhandler(void (*handler)(unsigned char scancode));
char kb_convertscancodetoascii(unsigned char scancode,char caps);
char kb_getkeystate(unsigned char scancode);
char kb_getcapslock();
char kb_getnumlock();
char kb_getscrolllock();
char kb_getcaps();


//SCREEN

void screen_putc(unsigned short x,unsigned short y,unsigned short c);
unsigned short screen_getc(unsigned short x,unsigned short y);
void screen_positioncursor(unsigned short x,unsigned short y);
void screen_putpixel(unsigned short x,unsigned short y,unsigned long p);

unsigned short screen_getwidth();
unsigned short screen_getheight();
unsigned long screen_getframebuffer();


//INITRD

void initrd_init();

#endif	/* KB_H */

