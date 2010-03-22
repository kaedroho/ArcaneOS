#include "video/video.h"
#include "libs/memory.h"

struct video_driver video_driverlist[3];
struct video_driver* video_currentdriver;
unsigned int video_currentdisplaymode;

//Function pointers
void (*video_putchar)(unsigned int x,unsigned int y,char character);
char (*video_getchar)(unsigned int x,unsigned int y);
void (*video_positioncursor)(unsigned int x,unsigned int y);
void (*video_putpixel)(unsigned int x,unsigned int y,unsigned int colour);
unsigned int (*video_getpixel)(unsigned int x,unsigned int y);
void (*video_cls)();

void video_init()
{
//Zero current video driver
    video_currentdriver=0;

//Zero driver list
    memset((unsigned char*)video_driverlist,0,sizeof(struct video_driver)*3);

//Setup driver list
    video_vgatext_init(&video_driverlist[0]);
    video_vgagraphics_init(&video_driverlist[1]);
    video_vesa_init(&video_driverlist[2]);
}

void video_setdriver(struct video_driver* Driver,unsigned int mode)
{
//Check if the driver has changed
    if(Driver!=video_currentdriver)
    {
    //End current driver
        if(video_currentdriver!=0)
            if(video_currentdriver->end!=0)
                video_currentdriver->end();

    //Start new driver
        if(Driver->start!=0)
            Driver->start(mode);

    //Set function pointers
        video_putchar=Driver->putchar;
        video_getchar=Driver->getchar;
        video_positioncursor=Driver->positioncursor;
        video_putpixel=Driver->putpixel;
        video_getpixel=Driver->getpixel;
        video_cls=Driver->cls;

    //Set current driver
        video_currentdriver=Driver;
    }
}

unsigned int video_getrows()
{
//Get text rows
    if(video_currentdriver!=0)
        return video_currentdriver->displaymode[video_currentdisplaymode].Rows;

//Return nothing
    return 0;
}

unsigned int video_getcollumns()
{
//Get text collumns
    if(video_currentdriver!=0)
        return video_currentdriver->displaymode[video_currentdisplaymode].Cols;

//Return nothing
    return 0;
}
