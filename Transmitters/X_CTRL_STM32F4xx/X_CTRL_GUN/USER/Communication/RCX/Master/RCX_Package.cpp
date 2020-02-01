#include "RCX.h"
#include "../crc.h"

static RCX::PackCommon_t PackCommon;

void RCX::SetObjectType(uint8_t objType)
{
    objType %= CtrlObject_MAX;
    PackCommon.Head.ObjectType = objType;
}

void RCX::SetObjectID(uint8_t objID)
{
    PackCommon.Head.ObjectID = objID;
}

void RCX::SetPackKey(uint8_t keyVal)
{
    PackCommon.Key = keyVal;
}

uint8_t RCX::GetObjectType()
{
    return PackCommon.Head.ObjectType;
}

int16_t* RCX::GetPackChannel()
{
    return (int16_t*)PackCommon.Channel;
}

void RCX::LoadPack(void* txbuff)
{
    PackCommon.Head.HeartBeat++;
    ChannelUpdate();
    PackCommon.CRC8 = crc8((const uint8_t*)&PackCommon, sizeof(PackCommon) - 1);
    *(PackCommon_t*)txbuff = PackCommon;
}
