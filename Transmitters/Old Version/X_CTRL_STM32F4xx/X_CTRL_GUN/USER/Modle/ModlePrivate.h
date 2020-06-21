#ifndef __MODLEPRIVATE_H
#define __MODLEPRIVATE_H

#include <stdint.h>

typedef struct{
    int16_t* pVal;
    const char* Name;
    int8_t AttachChannel;
}ChannelDisp_TypeDef;

extern ChannelDisp_TypeDef ChannelDisp_Grp[];

#endif
