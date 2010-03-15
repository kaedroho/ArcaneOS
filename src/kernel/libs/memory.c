#include "libs/memory.h"

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count)
{
//Variables
    int i;

//Copy data
    for(i=0;i<count;i++)
        dest[i]=src[i];

//Return destination
    return dest;
}

unsigned char *memset(unsigned char *dest, unsigned char val, int count)
{
//Variables
    int i;

//Fill data
    for(i=0;i<count;i++)
        dest[i]=val;

//Return destination
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
//Variables
    int i;

//Fill data
    for(i=0;i<count;i++)
        dest[i]=val;

//Return destination
    return dest;
}

char memcmp(char* stra,char* strb,unsigned int length)
{
//Test strings
    unsigned int i;
    for(i=0;i<length;i++)
        if(stra[i]!=strb[i])
            return 0;

//Return true
    return 1;
}

