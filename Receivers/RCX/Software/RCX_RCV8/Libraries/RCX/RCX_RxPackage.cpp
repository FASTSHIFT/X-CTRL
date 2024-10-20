#include "RCX.h"
#include "crc.h"

static RCX::PackCommon_t PackCommon;
static uint32_t RCX_LastRxTime;
static uint8_t RCX_ErrorCode = 0;

uint8_t RCX::GetRxPackKey()
{
    return PackCommon.Key;
}

bool RCX::GetRxConnected()
{
    return (RCX_ErrorCode == 0);
}

uint8_t RCX::GetRxPackErrorCode()
{
    return RCX_ErrorCode;
}

bool RCX::GetRxSignalLost()
{
    return (millis() - RCX_LastRxTime > RCX_SIGNAL_LOST_TIMEOUT);
}

int16_t RCX::GetRxPackChannel(uint8_t channel)
{
    return channel < RCX_CHANNEL_NUM ? PackCommon.Channel[channel] : 0;
}

void RCX::SetRxTimeUpdate()
{
    RCX_LastRxTime = millis();
}

static bool PackVerify(RCX::PackCommon_t* pack)
{
    static uint8_t lastHeartBeat;
    if(pack->Head.HeartBeat != lastHeartBeat)
    {
        RCX::SetRxTimeUpdate();
        lastHeartBeat = pack->Head.HeartBeat;
    }
    
    uint8_t errorCode = 0;

    if(millis() - RCX_LastRxTime > RCX_CONNECT_LOST_TIMEOUT)
    {
        errorCode |= RCX::EC_CONNECT_TIMEOUT;
    }
    
    if(pack->Head.ObjectType != RCX_TYPE)
    {
        errorCode |= RCX::EC_TYPE_ERROR;
    }
    
    if(pack->Head.ObjectID != RCX::Handshake::GetSlaveID())
    {
        errorCode |= RCX::EC_ID_ERROR;
    }
    
#if (RCX_USE_CRC == 1)
    if(pack->CRC8 != crc8((const uint8_t*)pack, sizeof(RCX::PackCommon_t) - 1))
    {
        errorCode |= RCX::EC_CRC_ERROR;
    }
#endif
    
    RCX_ErrorCode = errorCode;
    
    return (errorCode == 0);
}

bool RCX::LoadRxPack(void* rxbuff)
{
    bool retval = false;
    if(PackVerify((RCX::PackCommon_t*)rxbuff))
    {
        PackCommon = *(PackCommon_t*)rxbuff;
        retval = true;
    }
    return retval;
}
