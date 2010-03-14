#ifndef _DRAWER_H
#define _DRAWER_H

extern void draw_init();
extern void draw_setdrawcolour(unsigned int colour);
extern void draw_dotcolour(unsigned int x,unsigned int y,unsigned int colour);
extern void draw_dot(unsigned int x,unsigned int y);
extern void draw_linecolour(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int colour);
extern void draw_line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
extern void draw_boxcolour(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int colour);
extern void draw_box(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
extern void draw_filledboxcolour(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int colour);
extern void draw_filledbox(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
extern void draw_filledcirclecolour(unsigned int x,unsigned int y,unsigned int radius,unsigned int colour);
extern void draw_filledcircle(unsigned int x,unsigned int y,unsigned int radius);

#endif
