#ifndef _VIDEO_H
#define _VIDEO_H

#include "libs/rect.h"

struct video_driver
{
    unsigned int TextCols;
    unsigned int TextRows;
    unsigned int GraphicsCols;
    unsigned int GraphicsRows;

//Main functions
    void(*start)();
    void(*end)();

//Text functions
    void(*putchar)(unsigned int x,unsigned int y,char character);
    void(*positioncursor)(unsigned int x,unsigned int y);
    char(*getchar)(unsigned int x,unsigned int y);

//Graphics functions
    void(*putpixel)(unsigned int x,unsigned int y,unsigned int colour);
    unsigned int(*getpixel)(unsigned int x,unsigned int y);
};

//Main functions
void video_init();
void video_setdriver(struct video_driver* Driver);
void video_putchar(unsigned int x,unsigned int y,char character);
char video_getchar(unsigned int x,unsigned int y);
void video_positioncursor(unsigned int x,unsigned int y);
void video_putpixel(unsigned int x,unsigned int y,unsigned int colour);
unsigned int video_getpixel(unsigned int x,unsigned int y);
unsigned int video_getgraphicsrows();
unsigned int video_getgraphicscolumns();
unsigned int video_gettextrows();
unsigned int video_gettextcollumns();

//Text functions
struct video_driver* video_text_getdriver();
void video_text_init(struct video_driver* Driver);

//VGA functions
struct video_driver* video_vga_getdriver();
void video_vga_init(struct video_driver* Driver);

#endif
