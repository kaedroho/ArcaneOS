#include "sys.h"
#include "cli.h"
#include "libs/rect.h"
#include "video/video.h"

unsigned char g_CLI_DrawColour=0x0F;
unsigned char g_CLI_CursorX=0;
unsigned char g_CLI_CursorY=0;
struct Rect* g_CLI_Rect;

void cli_scrolldown()
{
//Variables
    unsigned int row;
    unsigned int collumn;

//Shift all rows but last one up
    for(row=cli_offsety();row<cli_height()+cli_offsety()-1;row++)
    {
        for(collumn=cli_offsetx();collumn<cli_width()+cli_offsetx();collumn++)
        {
            video_putchar(collumn,row,video_getchar(collumn,row+1));
            video_putpixel(collumn,row,video_getpixel(collumn,row+1));
        }
    }

//Clear bottom row
    for(collumn=cli_offsetx();collumn<cli_width()+cli_offsetx();collumn++)
        video_putchar(collumn,cli_offsety()+cli_height()-1,0);

//Move cursor up
    g_CLI_CursorY--;
}

void cli_putch(char character)
{
//Check if its a backspace
    if(character==0x08)
    {
        if(g_CLI_CursorX>cli_offsetx())
        {
            g_CLI_CursorX--;
            character=0;
        }else{
            return;
        }
    }

//Calculate position in to put the character
    unsigned int positionx=cli_offsetx()+g_CLI_CursorX;
    unsigned int positiony=cli_offsety()+g_CLI_CursorY;

//Check if the character is a new line
    if(character=='\n')
    {
    //New line and carriage return
        g_CLI_CursorY++;
        g_CLI_CursorX=cli_offsetx();
    }else if(character=='\r')
    {
    //Return
        g_CLI_CursorX=cli_offsetx();
    }else{
    //Put the character there
        video_putchar(positionx,positiony,character);
        video_putpixel(positionx,positiony,g_CLI_DrawColour);

    //Increase cursor position if the character is not null
        if(character!=0)
            g_CLI_CursorX++;

    //Check if the cursor has gone off the edge of the screen
        if(g_CLI_CursorX>cli_width()-1)
        {
            g_CLI_CursorY++;
            g_CLI_CursorX=cli_offsetx();
        }
    }

//Check if the cursor has gone off the bottom of the screen
    if(g_CLI_CursorY>cli_height()-1)
        cli_scrolldown();

//Update cursor
    video_positioncursor(cli_offsetx()+g_CLI_CursorX,cli_offsety()+g_CLI_CursorY);
}

void cli_puts(char* string)
{
//Print the string
    unsigned short strpos=0;
    while(string[strpos]!='\0')
    {
        cli_putch(string[strpos]);
        strpos++;
    }
}

void cli_putm(char* string,unsigned short length)
{
//Print the string
    unsigned int i;
    for(i=0;i<length;i++)
        cli_putch(string[i]);
}


void cli_putu32(unsigned int i, unsigned int base)
{
    char buffer[33] = {0};
    int j;
    for (j = 32; i || (j == 32); i /= base)
    {
         buffer[--j] = (i%base) + '0';
         if (buffer[j] > '9')
            buffer[j] = buffer[j] - '9' + 'A' - 1;
    }
    cli_puts(buffer+j);
}
void cli_puti32(int i, unsigned int base)
{
    char buffer[33] = {0};
    int j;

    int is_signed = i < 0;
    if (is_signed)
        i = -i;

    for (j = 32; i || (j == 32); i /= base)
    {
         buffer[--j] = (i%base) + '0';
         if (buffer[j] > '9')
            buffer[j] = buffer[j] - '9' + 'A' - 1;
    }

    if (is_signed)
        cli_putch('-');
    cli_puts(buffer+j);
}

void cli_settextcolour(char colour)
{
    g_CLI_DrawColour=colour;
}

void cli_cls()
{
//Variables
    unsigned short x;
    unsigned short y;

//Clear the screen
    for(x=0;x<video_getcollumns();x++)
        for(y=0;y<video_getrows();y++)
        {
            video_putpixel(x,y,g_CLI_DrawColour);
            video_putchar(x,y,0);
        }

}

void cli_clrect()
{
//Variables
    unsigned short x;
    unsigned short y;

//Clear the screen
    for(x=cli_offsetx();x<cli_offsetx()+cli_width();x++)
        for(y=cli_offsety();y<cli_offsety()+cli_height();y++)
        {
            video_putpixel(x,y,g_CLI_DrawColour);
            video_putchar(x,y,0);
        }

}

void cli_positioncursor(unsigned char x,unsigned char y)
{
//Position cursor
    g_CLI_CursorX=x;
    g_CLI_CursorY=y;

//Update cursor
    video_positioncursor(cli_offsetx()+g_CLI_CursorX,cli_offsety()+g_CLI_CursorY);
}

void cli_setrect(struct Rect* Rect)
{
//Set rect
    g_CLI_Rect=Rect;
}

unsigned int cli_width()
{
//Check if there is a rect
    if(g_CLI_Rect!=0)
        return GetRectWidth(g_CLI_Rect);

//Return standard width
    return 80;
}

unsigned int cli_height()
{
//Check if there is a rect
    if(g_CLI_Rect!=0)
        return GetRectHeight(g_CLI_Rect);

//Return standard height
    return 25;
}

unsigned int cli_offsetx()
{
//Check if there is a rect
    if(g_CLI_Rect!=0)
        return g_CLI_Rect->left;

//Return standard x offset
    return 0;
}

unsigned int cli_offsety()
{
//Check if there is a rect
    if(g_CLI_Rect!=0)
        return g_CLI_Rect->top;

//Return standard y offset
    return 0;
}
