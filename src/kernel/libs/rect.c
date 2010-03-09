#include "libs/rect.h"

struct Rect* BoundRect(struct Rect* Out,struct Rect* In,struct Rect* Bound)
{
//Bound the rect

//Return pointer
    return Out;
}

unsigned int GetRectWidth(struct Rect* In)
{
//Return width
    return In->right-In->left;
}

unsigned int GetRectHeight(struct Rect* In)
{
//Return height
    return In->bottom-In->top;
}
