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
  'B', 'N', 'M', '<', '>', '?', '\0', '*',
  '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', KB_KEY_HOME,
  KB_KEY_UP, KB_KEY_NPAGE, '-', KB_KEY_LEFT, '\0', KB_KEY_RIGHT, '+', KB_KEY_END,
  KB_KEY_DOWN, KB_KEY_PPAGE, '\0', KB_KEY_DC, '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', KB_OLPC_UP, KB_OLPC_DOWN, KB_OLPC_LEFT,
  KB_OLPC_RIGHT, '\0'
}
};


char kb_keypress[128];
char kb_capslock;
char kb_numlock;
char kb_scrolllock;
char kb_capslock_np;
char kb_numlock_np;
char kb_scrolllock_np;

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
    memset((unsigned char*)kb_keypress,0,128);
    kb_capslock=0;
    kb_numlock=0;
    kb_scrolllock=0;
    kb_capslock_np=0;
    kb_numlock_np=0;
    kb_scrolllock_np=0;

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
        kb_keypress[scancode&0x7F]=0;

    //Check if its caps lock
        if(scancode==0x3A)
            kb_capslock_np=0;

    //Check if its num block
        if(scancode==0x45)
            kb_numlock_np=0;

    //Check if its scroll lock
        if(scancode==0x46) //125
            kb_scrolllock_np=0;


    }else{
    //Click the key
        kb_keypress[scancode]=1;

    //TEMP: Put the ASCII character onto the screen
        console_putc(kb_convertscancodetoascii(scancode,kb_getcaps()));

    //Check if its caps lock
        if(scancode==0xBA && kb_capslock_np==0)
        {
            kb_capslock=1-kb_capslock;
            kb_capslock_np=1;

        //Update lights
            while((inb(0x64)&2)!=0);
            outb(0x60,0xED);
            while((inb(0x64)&2)!=0);
            outb(0x60,(kb_scrolllock&0x1)|((kb_numlock&0x1)<<1)|((kb_capslock&0x1)<<2));
        }

    //Check if its num block
        if(scancode==0xC5 && kb_numlock_np==0)
        {
            kb_numlock=1-kb_numlock;
            kb_numlock_np=1;

        //Update lights
            while((inb(0x64)&2)!=0);
            outb(0x60,0xED);
            while((inb(0x64)&2)!=0);
            outb(0x60,(kb_scrolllock&0x1)|((kb_numlock&0x1)<<1)|((kb_capslock&0x1)<<2));
        }

    //Check if its scroll lock
        if(scancode==0xC6 && kb_scrolllock_np==0) //125
        {
            kb_scrolllock=1-kb_scrolllock;
            kb_scrolllock_np=1;

        //Update lights
            while((inb(0x64)&2)!=0);
            outb(0x60,0xED);
            while((inb(0x64)&2)!=0);
            outb(0x60,(kb_scrolllock&0x1)|((kb_numlock&0x1)<<1)|((kb_capslock&0x1)<<2));
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
    return kb_keypress[scancode];
}

char kb_getcapslock()
{
    return kb_capslock;
}

char kb_getnumlock()
{
    return kb_numlock;
}

char kb_getscrolllock()
{
    return kb_scrolllock;
}

char kb_getcaps()
{
    return (((kb_capslock^(kb_keypress[0x36]|kb_keypress[0x2A]))&0x1)<<1)|((kb_keypress[0x36]|kb_keypress[0x2A])&0x1);
}