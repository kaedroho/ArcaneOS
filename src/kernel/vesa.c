#include "vesa.h"
#include "real.h"
#include "sys.h"
#include "paging.h"
#include "cli.h"

struct vesa_vbe_info* vesa_info;

void vesa_init()
{
    int handle_pg = pg_lock();
    vesa_info = vesa_get_vbe_info();
    if (!vesa_info)
        return;

    unsigned short* video_mode;
    for (video_mode = vesa_info->video_modes; *video_mode != 0xFFFF; video_mode++)
    {
        struct vesa_vbe_mode_info* mode_info = vesa_get_vbe_mode_info(*video_mode);
        if (mode_info)
        {
            // If this mode is supported, is a graphics mode, and supports a linear frame buffer
            if ((mode_info->attributes & 0x99) == 0x99)
            {
                cli_puts("\n"); cli_putu32(mode_info->Xres,10);
                cli_puts("x"); cli_putu32(mode_info->Yres,10);
                cli_puts(" @ "); cli_putu32(mode_info->bpp,10);
                cli_puts("bpp");
            }
        }
    }
    pg_unlock(handle_pg);
}

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
