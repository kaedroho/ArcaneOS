#include "libs/string.h"

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
