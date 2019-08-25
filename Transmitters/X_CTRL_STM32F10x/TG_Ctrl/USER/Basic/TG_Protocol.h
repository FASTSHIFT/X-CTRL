#ifndef __TG_PROTOCOL_H
#define	__TG_PROTOCOL_H

#include "RC_Protocol.h"

#pragma pack (1)

typedef struct{
    Protocol_Common_t Common;
    uint16_t KeyGrp;
}Protocol_CommonPlus_t;

#pragma pack ()

#endif
