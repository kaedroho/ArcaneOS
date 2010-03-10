#include "video/video.h"

struct video_driver* video_vga_driver;

struct video_driver* video_vga_getdriver()
{
    return video_vga_driver;
}

void video_vga_start()
{

}

void video_vga_end()
{

}

void video_vga_putpixel(unsigned int x,unsigned int y,unsigned int colour)
{

}

unsigned int video_vga_getpixel(unsigned int x,unsigned int y)
{
    return 0;
}

void video_vga_init(struct video_driver* Driver)
{
//Set variables
    Driver->TextCols=0;
    Driver->TextRows=0;
    Driver->GraphicsCols=320;
    Driver->GraphicsRows=200;
    Driver->start=video_vga_start;
    Driver->end=video_vga_end;
    Driver->putchar=0;
    Driver->getchar=0;
    Driver->positioncursor=0;
    Driver->putpixel=video_vga_putpixel;
    Driver->getpixel=video_vga_getpixel;

//Set driver
    video_vga_driver=Driver;
}
