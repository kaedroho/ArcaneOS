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


int strlen(char* str)
{
//Variables
    int length=0;

//Loop through string until a null character is found
    while(str[length]!='\0')
        length++;

//Return length
    return length;
}

char strcmp(char* stra,char* strb)
{
//Test lengths
    unsigned int Lengtha=strlen(stra);
    if(Lengtha!=strlen(strb))
        return 0;

//Test strings
    unsigned int i;
    for(i=0;i<Lengtha;i++)
        if(stra[i]!=strb[i])
            return 0;

//Return true
    return 1;
}

//Check if one of the strings is at the beginning of the other and return the distance in
unsigned int strcmpbegin(char* stra,char* strb)
{
    unsigned int i=0;
    while(stra[i]==strb[i])
        i++;
    if(stra[i]==0 || strb[i]==0)
        return i;
    return 0;
}