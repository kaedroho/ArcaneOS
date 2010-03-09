#include "sys.h"
#include "cli.h"
#include "libs/rect.h"

//TODO: Put the text parts of this file into video/text.c

unsigned char g_CLI_DrawColour=0x0F;
unsigned char g_CLI_CursorX=0;
unsigned char g_CLI_CursorY=0;
struct Rect* g_CLI_Rect;

void cli_updatecursor()
{
//Get cursor postion
    unsigned short position=(cli_offsetx()+g_CLI_CursorX)+(cli_offsety()+g_CLI_CursorY)*80;

//Send the position to the VGA controller
    outb(0x3D4,14);
    outb(0x3D5,position>>8);
    outb(0x3D4,15);
    outb(0x3D5,position);
}

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
            unsigned short* positiona=(unsigned short*)0xB8000;
            positiona=positiona+(collumn+row*80);
            unsigned short* positionb=(unsigned short*)0xB8000;
            positionb=positionb+(collumn+(row+1)*80);
            *positiona=*positionb;
        }
    }

//Clear bottom row
    for(collumn=cli_offsetx();collumn<cli_width()+cli_offsetx();collumn++)
    {
        unsigned short* position=(unsigned short*)0xB8000+collumn+(cli_offsety()+cli_height()-1)*80;
        *position=0;
    }

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
//Calculate position in memory to put the character
    unsigned short* position=(unsigned short*)0xB8000;
    position=position+((cli_offsetx()+g_CLI_CursorX)+(cli_offsety()+g_CLI_CursorY)*80);

//Check if the character is a new line
    if(character=='\n')
    {
    //New line and carriage return
        g_CLI_CursorY++;
        g_CLI_CursorX=cli_offsetx();
    }else if(character=='\r')
    {
    //Carriage return
        g_CLI_CursorX=cli_offsetx();
    }else{
    //Put the character there
        *position=g_CLI_DrawColour<<8|character;

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
    cli_updatecursor();
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

void cli_settextcolour(char colour)
{
    g_CLI_DrawColour=colour;
}

void cli_cls()
{
//Variables
    unsigned short* position=(unsigned short*)0xB8000;
    unsigned short i=0;

//Clear the screen
    for(i=0;i<2000;i++)
    {
        *position=g_CLI_DrawColour<<8;
        position++;
    }

}

void cli_clrect()
{
//Variables
    unsigned short* position=(unsigned short*)0xB8000;
    unsigned short x;
    unsigned short y;

//Clear the screen
    for(x=cli_offsetx();x<cli_offsetx()+cli_width();x++)
    {
        for(y=cli_offsety();y<cli_offsety()+cli_height();y++)
        {
            position=(unsigned short*)0xB8000+x+y*80;
            *position=g_CLI_DrawColour<<8;
        }
    }

}

void cli_positioncursor(unsigned char x,unsigned char y)
{
//Position cursor
    g_CLI_CursorX=x;
    g_CLI_CursorY=y;

//Update cursor
    cli_updatecursor();
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
