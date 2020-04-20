#include "RCX.h"

static bool AttachValue_Enable = true;

typedef struct
{
    int16_t* pValue;
    bool Reverse;
}ChannelGrp_t;

static ChannelGrp_t ChannelGrp[RCX_CHANNEL_NUM] = {0};

#ifndef constrain
#  define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

void RCX::ChannelSetAttachEnable(bool en)
{
    AttachValue_Enable = en;
    
    if(!en)
    {
        memset(ChannelGrp, NULL, sizeof(ChannelGrp));
    }
}

bool RCX::ChannelGetAttachEnable()
{
    return AttachValue_Enable;
}

int16_t RCX::ChannelGetAttachedIndex(int16_t* pValue)
{
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        if(pValue != NULL && ChannelGrp[i].pValue == pValue)
        {
            return i;
        }
    }
    return -1;
}

void RCX::ChannelSetAttach(uint8_t ch, int16_t* pValue)
{
    if(ch >= RCX_CHANNEL_NUM)
        return;

    int16_t index = ChannelGetAttachedIndex(pValue);
    if(index >= 0)
    {
        ChannelGrp[index].pValue = NULL;
    }
    ChannelGrp[ch].pValue = pValue;
}


int16_t* RCX::ChannelGetAttached(uint8_t ch)
{
    if(ch >= RCX_CHANNEL_NUM)
        return NULL;

    return ChannelGrp[ch].pValue;
}

void RCX::ChannelSetReverse(uint8_t ch, bool en)
{
    if(ch >= RCX_CHANNEL_NUM)
        return;
    
    ChannelGrp[ch].Reverse = en;
}

bool RCX::ChannelGetReverse(uint8_t ch)
{
    if(ch >= RCX_CHANNEL_NUM)
        return false;
    
    return ChannelGrp[ch].Reverse;
}

void RCX::ChannelWrite(uint8_t ch, int16_t value)
{
    if(ch >= RCX_CHANNEL_NUM)
        return;

    int16_t* txPackCh = GetTxPackChannel();
#if (RCX_CHANNEL_DATA_LIMIT == 1)
    value = constrain(value, -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX);
#endif
    txPackCh[ch] = ChannelGrp[ch].Reverse ? -value : value;
}

int16_t RCX::ChannelRead(uint8_t ch)
{
    if(ch >= RCX_CHANNEL_NUM)
        return 0;

    int16_t* txPackCh = GetTxPackChannel();
    return txPackCh[ch];
}

void RCX::ChannelUpdate()
{
    if(!AttachValue_Enable)
        return;

    int16_t* txPackCh = GetTxPackChannel();
    for(int ch = 0; ch < RCX_CHANNEL_NUM; ch++)
    {
        if(ChannelGrp[ch].pValue)
        {
            int16_t value;
#if (RCX_CHANNEL_DATA_LIMIT == 1)
            value = constrain(*(ChannelGrp[ch].pValue), -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX);
#else
            value = *(ChannelGrp[ch].pValue);
#endif
            txPackCh[ch] = ChannelGrp[ch].Reverse ? -value : value;
        }
    }
}

void RCX::ChannelReset()
{
    int16_t* txPackCh = GetTxPackChannel();
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        txPackCh[i] = 0;
    }
}
