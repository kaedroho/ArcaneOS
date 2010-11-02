#include <console.h>
#include <screen.h>
#include <string.h>

unsigned short g_console_cursorx=0;
unsigned short g_console_cursory=0;
unsigned char g_console_colour=0x0F;

unsigned char g_console_linelengths[25]={0};
unsigned char g_console_lineprotection[25]={0};

char g_console_commandbuffer[256]={0};
unsigned char g_console_commandbufferlength=0;

void _console_putc(char c,int input)
{
//Check that the character is viewable
    if(c>29 && c<128)
    {
    //Put character at cursor position
        screen_putc(g_console_cursorx,g_console_cursory,c|(g_console_colour<<8));
        g_console_linelengths[g_console_cursory]++;

    //Insert into command buffer
        if(input!=0)
        {
            g_console_commandbuffer[g_console_commandbufferlength]=c;
            g_console_commandbufferlength++;
        }

    //Find new cursor position
        g_console_cursorx++;
        if(g_console_cursorx==screen_getwidth())
        {
            g_console_cursory++;
            g_console_cursorx=g_console_lineprotection[g_console_cursory];
            if(g_console_cursory==screen_getheight())
            {
                g_console_cursory--;
                console_scrollup(1);
            }
        }
    }else{
        if(c=='\n')
        {
        //New line
            g_console_cursory++;
            g_console_cursorx=g_console_lineprotection[g_console_cursory];
            if(g_console_cursory==screen_getheight())
            {
                g_console_cursory--;
                console_scrollup(1);
            }

            //TEMP STUFF!!!!
            if(input!=0)
            {
                input=0;
                if(g_console_commandbuffer[0]!=0)
                {
                    console_puts_protected("OUT: ");
                    console_puts_protected(g_console_commandbuffer);
                    console_putc_protected('\n');
                }
                memset((unsigned char*)g_console_commandbuffer,0,256);
                g_console_commandbufferlength=0;
            }
            
        }
        if(c=='\r')
            g_console_cursorx=g_console_lineprotection[g_console_cursory];

        if(c=='\b')
        {
        //Remove last character from command buffer
            if(g_console_commandbufferlength>0)
            {
                g_console_commandbufferlength--;
                g_console_commandbuffer[g_console_commandbufferlength]=0;
            }

        //Position cursor
            if(g_console_cursorx==0)
            {
                if(g_console_cursory!=0)
                {
                    g_console_cursory--;
                    g_console_cursorx=g_console_linelengths[g_console_cursory];

                }
            }else{
                g_console_cursorx--;
            }

         //Check that this block is not protected
            if(g_console_lineprotection[g_console_cursory]>g_console_cursorx)
            {
            //Get the cursor out of the protected zone
                while(g_console_lineprotection[g_console_cursory]==screen_getwidth())
                    g_console_cursory++;
                g_console_cursorx=g_console_lineprotection[g_console_cursory];
            }else{
             //Clear character
                screen_putc(g_console_cursorx,g_console_cursory,'\0'|(g_console_colour<<8));
                if(g_console_cursorx!=0)
                    g_console_linelengths[g_console_cursory]--;
            }
        }
    }

//Position cursor
    screen_positioncursor(g_console_cursorx,g_console_cursory);

//Line protection
    if(input==0)
    {
        unsigned int i;
        for(i=0;i<g_console_cursory;i++)
        {
            g_console_lineprotection[i]=screen_getwidth();
        }
        g_console_lineprotection[g_console_cursory]=g_console_cursorx;
    }
}
void console_putc(char c)
{
    _console_putc(c,1);
}
void console_putc_protected(char c)
{
    _console_putc(c,0);
}

void console_puts(char* s)
{
//Variables
    int c=0;

//Loop through string until a null character is found
    while(s[c]!='\0')
    {
        console_putc(s[c]);
        c++;
    }
}
void console_puts_protected(char* s)
{
//Variables
    int c=0;

//Loop through string until a null character is found
    while(s[c]!='\0')
    {
        console_putc_protected(s[c]);
        c++;
    }
}


void console_putm(char* string,unsigned short length)
{
//Print the string
    unsigned int i;
    for(i=0;i<length;i++)
        console_putc(string[i]);
}
void console_putm_protected(char* string,unsigned short length)
{
//Print the string
    unsigned int i;
    for(i=0;i<length;i++)
        console_putc_protected(string[i]);
}


void console_putu32(unsigned int i, unsigned int base)
{
    char buffer[33] = {0};
    int j;
    for (j = 32; i || (j == 32); i /= base)
    {
         buffer[--j] = (i%base) + '0';
         if (buffer[j] > '9')
            buffer[j] = buffer[j] - '9' + 'A' - 1;
    }
    console_puts(buffer+j);
}
void console_putu32_protected(unsigned int i, unsigned int base)
{
    char buffer[33] = {0};
    int j;
    for (j = 32; i || (j == 32); i /= base)
    {
         buffer[--j] = (i%base) + '0';
         if (buffer[j] > '9')
            buffer[j] = buffer[j] - '9' + 'A' - 1;
    }
    console_puts_protected(buffer+j);
}


void console_puti32(int i, unsigned int base)
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
        console_putc('-');
    console_puts(buffer+j);
}
void console_puti32_protected(int i, unsigned int base)
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
        console_putc_protected('-');
    console_puts_protected(buffer+j);
}


void console_scrollup(unsigned short spaces)
{
    unsigned short y;
    for(y=0;y<screen_getheight();y++)
    {
        unsigned short x;
        for(x=0;x<screen_getwidth();x++)
            screen_putc(x,y,screen_getc(x,y+1));
        g_console_linelengths[y]=g_console_linelengths[y+1];
        g_console_lineprotection[y]=g_console_lineprotection[y+1];
    }
}