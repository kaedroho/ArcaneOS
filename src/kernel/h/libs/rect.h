#ifndef _RECT_H
#define _RECT_H

//Rect structure
struct Rect
{
    unsigned int left,right,top,bottom;
};

//Functions
struct Rect* BoundRect(struct Rect* Out,struct Rect* In,struct Rect* Bound);
unsigned int GetRectWidth(struct Rect* In);
unsigned int GetRectHeight(struct Rect* In);

#endif
