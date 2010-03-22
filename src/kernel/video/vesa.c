#include "video/video.h"
#include "video/vesa.h"
#include "sys.h"
#include "paging.h"
#include "mm.h"
#include "real.h"

struct video_driver* video_vesa_driver;

struct vesa_vbe_info* video_vesa_info;

struct video_displaymode* video_vesa_displaymodes;
struct vesa_vbe_mode_info** video_vesa_internaldisplaymodeinfo;
unsigned short* video_vesa_modenumbers;
unsigned int video_vesa_currentmode;


struct vesa_vbe_info* vesa_get_vbe_info()
{
    unsigned int handle_irq = irq_lock();
    unsigned int handle_pg = pg_lock();
    irq_unmap();

    struct vesa_vbe_info* result = (struct vesa_vbe_info*)real_call(real_get_vbe_info_id);

    irq_remap();
    pg_unlock(handle_pg);
    irq_unlock(handle_irq);

    return result;
}

struct vesa_vbe_mode_info* vesa_get_vbe_mode_info(unsigned short mode)
{
    unsigned int handle_irq = irq_lock();
    unsigned int handle_pg = pg_lock();
    irq_unmap();

    struct vesa_vbe_mode_info* result = (struct vesa_vbe_mode_info*)real_call(real_get_vbe_mode_info_id, mode);

    irq_remap();
    pg_unlock(handle_pg);
    irq_unlock(handle_irq);

    return result;
}

int vesa_set_vbe_mode(unsigned short mode)
{
    unsigned int handle_irq = irq_lock();
    unsigned int handle_pg = pg_lock();
    irq_unmap();

    int result = real_call(real_set_vbe_mode_id, mode);

    irq_remap();
    pg_unlock(handle_pg);
    irq_unlock(handle_irq);

    return result;
}




struct video_driver* video_vesa_getdriver()
{
    return video_vesa_driver;
}
#include "cli.h"

void video_vesa_start(unsigned int mode)
{
//video_vesa_currentmode=mode;
//vesa_set_vbe_mode(video_vesa_modenumbers[mode]);
cli_putu32(video_vesa_driver->displaymodecount,10);
}

void video_vesa_end()
{

}

void video_vesa_putpixel(unsigned int x,unsigned int y,unsigned int colour)
{
*(unsigned char*)(video_vesa_internaldisplaymodeinfo[video_vesa_currentmode]->physbase+video_vesa_internaldisplaymodeinfo[video_vesa_currentmode]->pitch*y+x)=colour&0xFF;
}

unsigned int video_vesa_getpixel(unsigned int x,unsigned int y)
{
    return 0;
}

void video_vesa_cls()
{

}

unsigned int video_vesa_getdisplaymodes()
{
    unsigned int modes=0;
    int handle_pg = pg_lock();
    video_vesa_info = vesa_get_vbe_info();
    if (!video_vesa_info)
        return 0;

    unsigned short* video_mode;

//Get mode count
    for (video_mode = video_vesa_info->video_modes; *video_mode != 0xFFFF; video_mode++)
    {
        struct vesa_vbe_mode_info* mode_info = vesa_get_vbe_mode_info(*video_mode);
        if (mode_info)
            if ((mode_info->attributes & 0x99) == 0x99)
                modes++;
    }

//Allocate array
    video_vesa_displaymodes=(struct video_displaymode*)mm_block_alloc(modes*sizeof(struct video_displaymode));
    video_vesa_internaldisplaymodeinfo=(struct vesa_vbe_mode_info**)mm_block_alloc(modes*sizeof(struct vesa_vbe_mode_info*));
    video_vesa_modenumbers=(unsigned short*)mm_block_alloc(modes*sizeof(unsigned short));

//Put modes into array
    unsigned int modeid=0;
    for (video_mode = video_vesa_info->video_modes; *video_mode != 0xFFFF; video_mode++)
    {
        struct vesa_vbe_mode_info* mode_info = vesa_get_vbe_mode_info(*video_mode);
        if (mode_info)
        {
            // If this mode is supported, is a graphics mode, and supports a linear frame buffer
            if ((mode_info->attributes & 0x99) == 0x99)
            {
                video_vesa_displaymodes[modeid].Type=3;
                video_vesa_displaymodes[modeid].Cols=mode_info->Xres;
                video_vesa_displaymodes[modeid].Rows=mode_info->Yres;
                video_vesa_displaymodes[modeid].Depth=mode_info->bpp;
                video_vesa_internaldisplaymodeinfo[modeid]=mode_info;
                video_vesa_modenumbers[modeid]=*video_mode;
                modeid++;
            }
        }
    }

    pg_unlock(handle_pg);

    return 0;//modes;
}

void video_vesa_init(struct video_driver* Driver)
{
//Set variables
    Driver->displaymodecount=video_vesa_getdisplaymodes();
    Driver->displaymode=video_vesa_displaymodes;
    Driver->start=video_vesa_start;
    Driver->end=video_vesa_end;
    Driver->putchar=0;
    Driver->getchar=0;
    Driver->positioncursor=0;
    Driver->putpixel=video_vesa_putpixel;
    Driver->getpixel=video_vesa_getpixel;
    Driver->cls=video_vesa_cls;

//Set driver
    video_vesa_driver=Driver;
}
