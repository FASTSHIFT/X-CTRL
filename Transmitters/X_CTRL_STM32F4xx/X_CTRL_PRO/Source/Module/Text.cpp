#include "Module.h"

static char GlobalTextBuff[5 * 1024];

char * TextGetBuff(uint32_t start)
{
    return (GlobalTextBuff + start);
}

uint32_t TextGetSize()
{
    return sizeof(GlobalTextBuff);
}

void TextClear(uint8_t n)
{
    for(uint32_t i = 0; i < sizeof(GlobalTextBuff); i++)
    {
        GlobalTextBuff[i] = n;
    }
}


