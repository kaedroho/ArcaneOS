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

}

void draw_dot(unsigned int x,unsigned int y)
{
//Call draw_dotcolour
    draw_dotcolour(x,y,g_drawcolour);
}

void draw_line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int colour)
{

}

void draw_line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
//Call draw_line
    draw_line(x1,y1,x2,y2,g_drawcolour)
}
