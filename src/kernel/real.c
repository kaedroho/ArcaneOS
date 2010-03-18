#include "sys.h"
#include "real.h"
#include "libs/memory.h"
#include "paging.h"

#define real_offset 0x9000 // 36 KB
#define real_size 172
#define real_dest 0x7C00

unsigned int (*real_call)(unsigned int id, ...) = (unsigned int (*)(unsigned int id, ...))real_dest;

void real_init()
{
    pg_set_enabled(0);
    memcpy((unsigned char*)real_dest, (unsigned char*)real_offset, real_size);
    pg_set_enabled(1);
}

