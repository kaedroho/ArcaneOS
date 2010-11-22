#include <console.h>
#include <x86/ibmpc.h>
#include <string.h>
#include <x86/sys.h>
#include <x86/vbe_real.h>

unsigned short g_console_cursorx=0;
unsigned short g_console_cursory=0;
unsigned char g_console_colour=0x0F;

unsigned char g_console_linelengths[25]={0};
unsigned char g_console_lineprotection[25]={0};

char g_console_commandbuffer[256]={0};
unsigned char g_console_commandbufferlength=0;

void console_listvbemodes(char* params)
{
    if(strcmp(params," -list")){
        struct vbe_info_block* info = (struct vbe_info_block*)mm_low_alloc(sizeof(struct vbe_info_block));

        vbe_get_controller_info(info);

        console_puts_protected("VBE Modes:\n");
        unsigned short* modes = info->video_mode_ptr;

        struct vbe_mode_info_block* mode_info=(struct vbe_mode_info_block*)mm_low_alloc(sizeof(struct vbe_mode_info_block));
        while (*modes != 0xFFFF) {
            console_puts_protected("0x");
            int mode=*(modes++);
            console_putu32_protected(mode, 16);
            console_puts_protected(": ");
            vbe_get_mode_info(mode,mode_info);
            console_putu32_protected(mode_info->x_resolution, 10);
            console_puts_protected("x");
            console_putu32_protected(mode_info->y_resolution, 10);
            console_puts_protected("x");
            console_putu32_protected(mode_info->bits_per_pixel, 10);
            console_puts_protected(" @ 0x");
            console_putu32_protected(mode_info->phys_base_addr, 16);
            console_puts_protected("\n");

        }
        mm_low_free((unsigned char*)mode_info);
        console_puts_protected("\n");

        mm_low_free((unsigned char*)info);
        return;
    }
    if(strcmpbegin(params," -set")==5){
        unsigned int length=strlen(params+8);
        unsigned int mode=0,i;
        for(i=0;i<length;i++){
            if(params[8+i]=='1') mode|=(0x1<<(length-i-1)*4);
            if(params[8+i]=='2') mode|=(0x2<<(length-i-1)*4);
            if(params[8+i]=='3') mode|=(0x3<<(length-i-1)*4);
            if(params[8+i]=='4') mode|=(0x4<<(length-i-1)*4);
            if(params[8+i]=='5') mode|=(0x5<<(length-i-1)*4);
            if(params[8+i]=='6') mode|=(0x6<<(length-i-1)*4);
            if(params[8+i]=='7') mode|=(0x7<<(length-i-1)*4);
            if(params[8+i]=='8') mode|=(0x8<<(length-i-1)*4);
            if(params[8+i]=='9') mode|=(0x9<<(length-i-1)*4);
            if(params[8+i]=='A') mode|=(0xA<<(length-i-1)*4);
            if(params[8+i]=='B') mode|=(0xB<<(length-i-1)*4);
            if(params[8+i]=='C') mode|=(0xC<<(length-i-1)*4);
            if(params[8+i]=='D') mode|=(0xD<<(length-i-1)*4);
            if(params[8+i]=='E') mode|=(0xE<<(length-i-1)*4);
            if(params[8+i]=='F') mode|=(0xF<<(length-i-1)*4);
            if(params[8+i]=='a') mode|=(0xA<<(length-i-1)*4);
            if(params[8+i]=='b') mode|=(0xB<<(length-i-1)*4);
            if(params[8+i]=='c') mode|=(0xC<<(length-i-1)*4);
            if(params[8+i]=='d') mode|=(0xD<<(length-i-1)*4);
            if(params[8+i]=='e') mode|=(0xE<<(length-i-1)*4);
            if(params[8+i]=='f') mode|=(0xF<<(length-i-1)*4);
        }
        vbe_set_mode(mode,0);
    }
}


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
                    if(strcmpbegin(g_console_commandbuffer,"vbe")==3 || strcmp(g_console_commandbuffer,"vbe")){
                        console_listvbemodes(&g_console_commandbuffer[3]);
                    }else{
                        console_puts_protected("OUT: ");
                        console_puts_protected(g_console_commandbuffer);
                        console_putc_protected('\n');
                    }
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

void console_error()
{
    g_console_colour=0x4F;
    g_console_cursorx=g_console_cursory=0;
    unsigned short y,x;
    for(y=0;y<screen_getheight();y++){
        for(x=0;x<screen_getwidth();x++){
            screen_putc(x,y,(g_console_colour<<8));
        }
        g_console_lineprotection[y]=0;
        g_console_linelengths[y]=0;
    }
}