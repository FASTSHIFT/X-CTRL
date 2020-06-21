#include "RCX.h"

#if (RCX_USE_CRC == 1)
#  include "crc.h"
#endif

static RCX::PackCommon_t PackCommon;

void RCX::TxSetObjectType(uint8_t objType)
{
    PackCommon.Head.ObjectType = objType;
}

void RCX::TxSetObjectID(uint8_t objID)
{
    PackCommon.Head.ObjectID = objID;
}

void RCX::TxSetPackKey(uint8_t keyVal)
{
    PackCommon.Key = keyVal;
}

uint8_t RCX::TxGetObjectType()
{
    return PackCommon.Head.ObjectType;
}

uint8_t RCX::TxGetObjectID()
{
    return PackCommon.Head.ObjectID;
}

int16_t* RCX::TxGetPackChannel()
{
    return (int16_t*)PackCommon.Channel;
}

void RCX::TxLoadPack(void* txbuff)
{
    PackCommon.Head.HeartBeat++;
    ChannelUpdate();
#if (RCX_USE_CRC == 1)
    PackCommon.CRC8 = crc8((const uint8_t*)&PackCommon, sizeof(PackCommon) - 1);
#endif
    *(PackCommon_t*)txbuff = PackCommon;
}
