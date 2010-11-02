#ifndef KB_H
#define	KB_H

void kb_init();
void kb_setlayout(char* layout);
void kb_hitbutton(unsigned char scancode);
void kb_setkeyboardhandler(void (*handler)(unsigned char scancode));
char kb_convertscancodetoascii(unsigned char scancode,char caps);
char kb_getkeystate(unsigned char scancode);
char kb_getcapslock();
char kb_getnumlock();
char kb_getscrolllock();
char kb_getcaps();

#endif	/* KB_H */

