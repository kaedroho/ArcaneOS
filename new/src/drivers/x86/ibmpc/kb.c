#include <sys-x86.h>
#include <kb.h>
#include <string.h>
#include <console.h>

#define CAPS_SYMBOLSANDNUMBERS 1
#define CAPS_LETTERS 2


//US keyboard layout
char KB_DefaultLayout[256] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */

    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

char g_KB_Keypress[128];
char g_KB_CapsLock;
char g_KB_NumLock;
char g_KB_ScrollLock;
char g_KB_CapsLockNP;
char g_KB_NumLockNP;
char g_KB_ScrollLockNP;

char* g_KB_Layout;

void (*g_kb_handler)(unsigned char scancode) = 0;

void kb_handler(struct regs *r)
{
//Send button to kb driver
    kb_hitbutton(inb(0x60));
}

void kb_init()
{
//Install keyboard handler
    irq_install_handler(1,kb_handler);

//Set default layout
    kb_setlayout(KB_DefaultLayout);

//Zero keystates
    memset((unsigned char*)g_KB_Keypress,0,128);
    g_KB_CapsLock=0;
    g_KB_NumLock=0;
    g_KB_ScrollLock=0;
    g_KB_CapsLockNP=0;
    g_KB_NumLockNP=0;
    g_KB_ScrollLockNP=0;

    console_puts_protected("KEYBOARD: Started. Using US Keyboard layout.\n");

}

void kb_setlayout(char* layout)
{
    g_KB_Layout=layout;
}

void kb_hitbutton(unsigned char scancode)
{
//TEMP: Put character onto the screen
    if(scancode&0x80)
    {
    //Unclick the key
        g_KB_Keypress[scancode&0x7F]=0;

    //Check if its caps lock
        if(scancode==0x3A)
            g_KB_CapsLockNP=0;

    //Check if its num block
        if(scancode==0x45)
            g_KB_NumLockNP=0;

    //Check if its scroll lock
        if(scancode==0x46) //125
            g_KB_ScrollLockNP=0;


    }else{
    //Say that the key has been pressed
        g_KB_Keypress[scancode]=1;

    //TEMP: Put the ASCII character onto the screen
        console_putc(kb_convertscancodetoascii(scancode,kb_getcaps()));

    //Check if its caps lock
        if(scancode==0xBA && g_KB_CapsLockNP==0)
        {
            g_KB_CapsLock=1-g_KB_CapsLock;
            g_KB_CapsLockNP=1;

        //Update lights
            while((inb(0x64)&2)!=0);
            outb(0x60,0xED);
            while((inb(0x64)&2)!=0);
            outb(0x60,(g_KB_ScrollLock&0x1)|((g_KB_NumLock&0x1)<<1)|((g_KB_CapsLock&0x1)<<2));
        }

    //Check if its num block
        if(scancode==0xC5 && g_KB_NumLockNP==0)
        {
            g_KB_NumLock=1-g_KB_NumLock;
            g_KB_NumLockNP=1;

        //Update lights
            while((inb(0x64)&2)!=0);
            outb(0x60,0xED);
            while((inb(0x64)&2)!=0);
            outb(0x60,(g_KB_ScrollLock&0x1)|((g_KB_NumLock&0x1)<<1)|((g_KB_CapsLock&0x1)<<2));
        }

    //Check if its scroll lock
        if(scancode==0xC6 && g_KB_ScrollLockNP==0) //125
        {
            g_KB_ScrollLock=1-g_KB_ScrollLock;
            g_KB_ScrollLockNP=1;

        //Update lights
            while((inb(0x64)&2)!=0);
            outb(0x60,0xED);
            while((inb(0x64)&2)!=0);
            outb(0x60,(g_KB_ScrollLock&0x1)|((g_KB_NumLock&0x1)<<1)|((g_KB_CapsLock&0x1)<<2));
        }
    }

//Check if there is a keyboard handler
    if(g_kb_handler!=0)
    {
    //Call the keyboard handler
        g_kb_handler(scancode);
    }
}

void kb_setkeyboardhandler(void (*handler)(unsigned char scancode))
{
    g_kb_handler=handler;
}

char kb_convertscancodetoascii(unsigned char scancode,char caps)
{
//Return ASCII code
    return g_KB_Layout[scancode+((caps&0x2)?90:0)];
}

char kb_getkeystate(unsigned char scancode)
{
    return g_KB_Keypress[scancode];
}

char kb_getcapslock()
{
    return g_KB_CapsLock;
}

char kb_getnumlock()
{
    return g_KB_NumLock;
}

char kb_getscrolllock()
{
    return g_KB_ScrollLock;
}

char kb_getcaps()
{
    return (((g_KB_CapsLock^(g_KB_Keypress[0x36]|g_KB_Keypress[0x2A]))&0x1)<<1)|((g_KB_Keypress[0x36]|g_KB_Keypress[0x2A])&0x1);
}