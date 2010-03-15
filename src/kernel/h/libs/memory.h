#ifndef _STRING_H
#define _STRING_H

extern unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);
extern unsigned char *memset(unsigned char *dest, unsigned char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern char memcmp(char* stra,char* strb,unsigned int length);
#endif
