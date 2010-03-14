#ifndef _VGABASE_H
#define _VGABASE_H

//Text functions
struct video_driver* video_vgatext_getdriver();
void video_vgatext_init(struct video_driver* Driver);

//Graphics functions
struct video_driver* video_vgagraphics_getdriver();
void video_vgagraphics_init(struct video_driver* Driver);

//VGA Base functions
extern void vga_writeregs(unsigned char *regs);
extern void vga_setplane(unsigned int p);
extern unsigned vga_getframebuffersegment(void);

//Font functions
extern unsigned char g_8x8_font[2048];
extern unsigned char g_8x16_font[4096];
void vga_writefont(unsigned char *buf, unsigned font_height);

#endif
