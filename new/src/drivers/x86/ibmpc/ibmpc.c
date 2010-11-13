#include <x86/ibmpc.h>
#include <vfs.h>

void ibmpc_init()
{
//Initialise user input
    kb_init();
    
//Initialise filesystems
    initrd_init();
}