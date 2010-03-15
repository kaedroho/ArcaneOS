#ifndef _VIDEO_H
#define _VIDEO_H

#include "libs/rect.h"
#include "video/vgabase.h"

struct video_displaymode
{
    unsigned char Type; //1=text, 2=text with background, 3=graphics
    unsigned int Cols;
    unsigned int Rows;
    unsigned char Depth;
};

struct video_driver
{
//Display mode list
    struct video_displaymode* displaymode;
    unsigned int displaymodecount;

//Main functions
    void(*start)(unsigned int mode);
    void(*end)();

//Text functions
    void(*putchar)(unsigned int x,unsigned int y,char character);
    void(*positioncursor)(unsigned int x,unsigned int y);
    char(*getchar)(unsigned int x,unsigned int y);

//Graphics functions
    void(*putpixel)(unsigned int x,unsigned int y,unsigned int colour);
    unsigned int(*getpixel)(unsigned int x,unsigned int y);
    void(*cls)();
};

//Main functions
extern void video_init();
extern void video_setdriver(struct video_driver* Driver,unsigned int mode);
extern unsigned int video_getrows();
extern unsigned int video_getcollumns();

//Function pointers
extern void (*video_putchar)(unsigned int x,unsigned int y,char character);
extern char (*video_getchar)(unsigned int x,unsigned int y);
extern void (*video_positioncursor)(unsigned int x,unsigned int y);
extern void (*video_putpixel)(unsigned int x,unsigned int y,unsigned int colour);
extern unsigned int (*video_getpixel)(unsigned int x,unsigned int y);
extern void (*video_cls)();
#endif
