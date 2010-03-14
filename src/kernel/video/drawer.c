#include "video/video.h"
#include "video/drawer.h"

unsigned int g_drawcolour;

void draw_init()
{
    g_drawcolour=0xFFFFFFFF;
}

void draw_setdrawcolour(unsigned int colour)
{
    g_drawcolour=colour;
}

void draw_dotcolour(unsigned int x,unsigned int y,unsigned int colour)
{
//Put a single pixel
    video_putpixel(x,y,colour);
}

void draw_dot(unsigned int x,unsigned int y)
{
//Call draw_dotcolour
    draw_dotcolour(x,y,g_drawcolour);
}


void draw_linecolour(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int colour)
{

}

void draw_line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
//Call draw_linecolour
    draw_linecolour(x1,y1,x2,y2,g_drawcolour);
}


void draw_boxcolour(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int colour)
{

}

void draw_box(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
//Call draw_boxcolour
    draw_boxcolour(x1,y1,x2,y2,g_drawcolour);
}


void draw_filledboxcolour(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int colour)
{
//Loop through pixels and put them
    unsigned int x,y;
    for(x=x1;x<x2+1;x++)
        for(y=y1;y<y2+1;y++)
            video_putpixel(x,y,colour);
}

void draw_filledbox(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
//Call draw_filledboxcolour
    draw_filledboxcolour(x1,y1,x2,y2,g_drawcolour);
}


void draw_filledcirclecolour(unsigned int x,unsigned int y,unsigned int radius,unsigned int colour)
{

}

void draw_filledcircle(unsigned int x,unsigned int y,unsigned int radius)
{
//Call draw_filledcirclecolour
    draw_filledcirclecolour(x,y,radius,g_drawcolour);
}
