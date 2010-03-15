#ifndef _CLI_H
#define _CLI_H

#include "libs/rect.h"

extern void cli_putch(char character);
extern void cli_puts(char* string);
extern void cli_putm(char* string,unsigned short length);
extern void cli_putu32(unsigned int i, unsigned int base);
extern void cli_puti32(int i, unsigned int base);
extern void cli_settextcolour(char colour);
extern void cli_cls();
extern void cli_clrect();
extern void cli_positioncursor(unsigned char x,unsigned char y);
extern void cli_setrect(struct Rect* Rect);
extern unsigned int cli_width();
extern unsigned int cli_height();
extern unsigned int cli_offsetx();
extern unsigned int cli_offsety();
#endif
