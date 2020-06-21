#include "RCX.h"
#include "crc.h"

static RCX::PackCommon_t PackCommon;

void RCX::SetTxObjectType(uint8_t objType)
{
    PackCommon.Head.ObjectType = objType;
}

void RCX::SetTxObjectID(uint8_t objID)
{
    PackCommon.Head.ObjectID = objID;
}

void RCX::SetTxPackKey(uint8_t keyVal)
{
    PackCommon.Key = keyVal;
}

uint8_t RCX::GetTxObjectType()
{
    return PackCommon.Head.ObjectType;
}

uint8_t RCX::GetTxObjectID()
{
    return PackCommon.Head.ObjectID;
}

int16_t* RCX::GetTxPackChannel()
{
    return (int16_t*)PackCommon.Channel;
}

void RCX::LoadTxPack(void* txbuff)
{
    PackCommon.Head.HeartBeat++;
    ChannelUpdate();
#if (RCX_USE_CRC == 1)
    PackCommon.CRC8 = crc8((const uint8_t*)&PackCommon, sizeof(PackCommon) - 1);
#endif
    *(PackCommon_t*)txbuff = PackCommon;
}
