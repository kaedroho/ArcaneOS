#include "video/video.h"
#include "sys.h"

struct video_driver* video_text_driver;

struct video_driver* video_text_getdriver()
{
    return video_text_driver;
}

void video_text_putchar(unsigned int x,unsigned int y,char character)
{
//Calculate position
    unsigned char* position=(unsigned char*)0xB8000+(x+y*80)*2;

//Put character
    *position=character;
}

char video_text_getchar(unsigned int x,unsigned int y)
{
    return 0;
}

void video_text_positioncursor(unsigned int x,unsigned int y)
{
//Get cursor postion
    unsigned short position=x+y*80;

//Send the position to the VGA controller
    outb(0x3D4,14);
    outb(0x3D5,position>>8);
    outb(0x3D4,15);
    outb(0x3D5,position);
}

void video_text_putpixel(unsigned int x,unsigned int y,unsigned int colour)
{
//Calculate position
    unsigned char* position=(unsigned char*)0xB8000+(x+y*80)*2+1;

//Put character
    *position=colour;
}

unsigned int video_text_getpixel(unsigned int x,unsigned int y)
{
    return 0;
}

void video_text_init(struct video_driver* Driver)
{
//Set variables
    Driver->TextCols=80;
    Driver->TextRows=25;
    Driver->GraphicsCols=80;
    Driver->GraphicsRows=25;
    Driver->start=0;
    Driver->end=0;
    Driver->putchar=video_text_putchar;
    Driver->getchar=video_text_getchar;
    Driver->positioncursor=video_text_positioncursor;
    Driver->putpixel=video_text_putpixel;
    Driver->getpixel=video_text_getpixel;

//Set driver
    video_text_driver=Driver;
}
