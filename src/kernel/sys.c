unsigned char inb(unsigned short port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "d" (port));
    return rv;
}

void outb(unsigned short port,unsigned char data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "d" (port), "a" (data));
}

unsigned short inw(unsigned short port)
{
    unsigned short rv;
    __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "d" (port));
    return rv;
}

void outw(unsigned short port,unsigned short data)
{
    __asm__ __volatile__ ("outw %1, %0" : : "d" (port), "a" (data));
}
/*
unsigned long inl(unsigned short port)
{
    unsigned long rv;
    __asm__ __volatile__ ("inl %1, %0" : "=a" (rv) : "d" (port));
    return rv;
}
*/
void outl(unsigned short port,unsigned long data)
{
    __asm__ __volatile__ ("outl %1, %0" : : "d" (port), "a" (data));
}

unsigned char peekb(unsigned short seg,unsigned short off)
{
    return *(unsigned char*)(seg*16+off);
}

void pokeb(unsigned short seg,unsigned short off,unsigned char val)
{
    *(unsigned char*)(seg*16+off)=val;
}

unsigned short peekw(unsigned short seg,unsigned short off)
{
    return *(unsigned short*)(seg*16+off);
}

void pokew(unsigned short seg,unsigned short off,unsigned short val)
{
    *(unsigned short*)(seg*16+off)=val;
}

unsigned long peekl(unsigned short seg,unsigned short off)
{
    return *(unsigned long*)(seg*16+off);
}

void pokel(unsigned short seg,unsigned short off,unsigned long val)
{
    *(unsigned long*)(seg*16+off)=val;
}
