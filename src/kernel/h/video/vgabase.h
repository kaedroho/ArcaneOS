#ifndef _VGABASE_H
#define _VGABASE_H

//Text functions
struct video_driver* video_vgatext_getdriver();
void video_vgatext_init(struct video_driver* Driver);

//VGA functions
struct video_driver* video_vgagraphics_getdriver();
void video_vgagraphics_init(struct video_driver* Driver);

//VGA Base functions
extern void vga_writeregs(unsigned char *regs);
extern void vga_setplane(unsigned int p);
extern unsigned vga_getframebuffersegment(void);
#endif
