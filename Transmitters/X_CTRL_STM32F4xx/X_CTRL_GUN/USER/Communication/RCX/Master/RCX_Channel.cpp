#include "RCX.h"

using namespace RCX;

static bool AttachValue_Enable = true;
static int16_t* ChannelValReg_Grp[RCX_CHANNEL_NUM];

void RCX::ChannelAttachValueSetEnable(bool en)
{
    AttachValue_Enable = en;
    
    if(!en)
    {
        memset(ChannelValReg_Grp, NULL, sizeof(ChannelValReg_Grp));
    }
}

bool RCX::ChannelAttachValueGetEnable()
{
    return AttachValue_Enable;
}

int16_t RCX::ChannelCheckValue(int16_t* pValue)
{
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        if(pValue != NULL && ChannelValReg_Grp[i] == pValue)
        {
            return i;
        }
    }
    return -1;
}

void RCX::ChannelAttachValue(uint8_t ch, int16_t* pValue)
{
    if(ch >= RCX_CHANNEL_NUM)
        return;

    int16_t index = ChannelCheckValue(pValue);
    if(index >= 0)
    {
        ChannelValReg_Grp[index] = NULL;
    }
    ChannelValReg_Grp[ch] = pValue;
}


bool RCX::ChannelGetAttached(uint8_t ch)
{
    if(ch >= RCX_CHANNEL_NUM)
        return false;

    return ChannelValReg_Grp[ch] == NULL ? false : true;
}

int16_t** RCX::ChannelGetValRegList()
{
    return ChannelValReg_Grp;
}

void RCX::ChannelWriteValue(uint8_t ch, int16_t value)
{
    if(ch >= RCX_CHANNEL_NUM)
        return;

    int16_t* chGrp = GetPackChannel();
    chGrp[ch] = constrain(value, -RCX_ChannelData_Max, RCX_ChannelData_Max);
}

int16_t RCX::ChannelReadValue(uint8_t ch)
{
    if(ch >= RCX_CHANNEL_NUM)
        return 0;

    int16_t* chGrp = GetPackChannel();
    return chGrp[ch];
}

void RCX::ChannelUpdate()
{
    if(!AttachValue_Enable)
        return;

    int16_t* chGrp = GetPackChannel();
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        if(ChannelValReg_Grp[i])
        {
            chGrp[i] = constrain(*(ChannelValReg_Grp[i]), -RCX_ChannelData_Max, RCX_ChannelData_Max);
        }
    }
}

void RCX::ChannelReset()
{
    int16_t* chGrp = GetPackChannel();
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        chGrp[i] = 0;
    }
}
