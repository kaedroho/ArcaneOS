unsigned char inb(unsigned short port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

void outb(unsigned short port,unsigned char data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
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
