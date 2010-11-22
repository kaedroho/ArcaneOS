#ifndef STRING_H
#define	STRING_H


unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);
unsigned char *memset(unsigned char *dest, unsigned char val, int count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
char memcmp(char* stra,char* strb,unsigned int length);

unsigned int strlen(char* str);
char strcmp(char* stra,char* strb);
unsigned int strcmpbegin(char* stra,char* strb);
unsigned int strcpy(char* src,char* dest);


#endif	/* STRING_H */

