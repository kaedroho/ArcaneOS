#include "sys.h"
#include "real.h"
#include "libs/memory.h"
#include "paging.h"

extern void real_start();
extern void real_end();

unsigned int (*real_call)(unsigned short id, ...) = (unsigned int (*)(unsigned short id, ...))0x7C00;

void real_init()
{
    // Load the real mode library
    memcpy((unsigned char*)0x7C00, (unsigned char*)&real_start, (unsigned int)&real_end - (unsigned int)&real_start);
}

