#ifndef __RCX_TYPE_H
#define __RCX_TYPE_H

#include "stdint.h"

#include "RCX_Config.h"

#if( RCX_CHANNEL_NUM <= 0 || RCX_CHANNEL_NUM > 12 )
#  error "RCX_CHANNEL_NUM must >0 and <= 12"
#endif

namespace RCX
{

/*强制单字节对齐*/
#pragma pack (1)

/*通用通信格式 (32 Bytes)*/
typedef struct
{
    /*数据包头 3 Bytes*/
    struct
    {
        uint8_t ObjectType;
        uint8_t ObjectID;
        uint8_t HeartBeat;
    } Head;

    /*按键 1 Byte*/
    uint8_t Key;

    /*通道 RCX_CHANNEL_NUM * 2 Bytes*/
    int16_t Channel[RCX_CHANNEL_NUM];
    
    /*用户数据*/
    uint8_t UserData[27 - (RCX_CHANNEL_NUM * sizeof(uint16_t))];

    /*CRC校验码 1 Byte*/
    uint8_t CRC8;
} PackCommon_t;

#pragma pack ()

}/*end of namespace RCX*/

#endif
