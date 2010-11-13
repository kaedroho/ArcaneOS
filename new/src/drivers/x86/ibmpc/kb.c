#include <x86/sys.h>
#include <x86/ibmpc.h>
#include <string.h>
#include <console.h>

#define CAPS_SYMBOLSANDNUMBERS 1
#define CAPS_LETTERS 2

// Set these to unused value if use is required
#define KB_KEY_ESC '\0'
#define KB_KEY_BACKSPACE '\b'
#define KB_KEY_TAB '\t'
#define KB_KEY_HOME '\0'
#define KB_KEY_UP '\0'
#define KB_KEY_NPAGE '\0'
#define KB_KEY_LEFT '\0'
#define KB_KEY_RIGHT '\0'
#define KB_KEY_END '\0'
#define KB_KEY_DOWN '\0'
#define KB_KEY_PPAGE '\0'
#define KB_KEY_DC '\0'
#define KB_OLPC_UP '\0'
#define KB_OLPC_DOWN '\0'
#define KB_OLPC_LEFT '\0'
#define KB_OLPC_RIGHT '\0'

//US keyboard layout
struct KB_Layout KB_DefaultLayout = {
{   // Without shift
  '\0', KB_KEY_ESC, '1', '2', '3', '4', '5', '6',
  '7', '8', '9', '0', '-', '=', KB_KEY_BACKSPACE, KB_KEY_TAB,
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
  'o', 'p', '[', ']', '\n', '\0', 'a', 's',
  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',', '.', '/', '\0', '*',
  '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', KB_KEY_HOME,
  KB_KEY_UP, KB_KEY_NPAGE, '-', KB_KEY_LEFT, '\0', KB_KEY_RIGHT, '+', KB_KEY_END,
  KB_KEY_DOWN, KB_KEY_PPAGE, '\0', KB_KEY_DC, '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', KB_OLPC_UP, KB_OLPC_DOWN, KB_OLPC_LEFT,
  KB_OLPC_RIGHT, '\0'
},
{   // With shift
  '\0', '\0', '!', '@', '#', '$', '%', '^',
  '&', '*', '(', ')', '_', '+', '\0', '\0',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
  'O', 'P', '{', '}', '\n', '\0', 'A', 'S',
  'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '\"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?', '\0'
}
};


char g_KB_Keypress[128];
char g_KB_CapsLock;
char g_KB_NumLock;
char g_KB_ScrollLock;
char g_KB_CapsLockNP;
char g_KB_NumLockNP;
char g_KB_ScrollLockNP;

struct KB_Layout* g_KB_Layout;

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
    kb_setlayout(&KB_DefaultLayout);

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

void kb_setlayout(struct KB_Layout* layout)
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
    if (caps&0x2)
        return g_KB_Layout->shiftedMapping[scancode];
    else
        return g_KB_Layout->defaultMapping[scancode];
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