#include "RCX.h"

using namespace RCX;

static bool AttachValue_Enable = true;
static int16_t* ChannelReg_Grp[RCX_CHANNEL_NUM];

void RCX::ChannelSetAttachEnable(bool en)
{
    AttachValue_Enable = en;
    
    if(!en)
    {
        memset(ChannelReg_Grp, NULL, sizeof(ChannelReg_Grp));
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
        if(pValue != NULL && ChannelReg_Grp[i] == pValue)
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
        ChannelReg_Grp[index] = NULL;
    }
    ChannelReg_Grp[ch] = pValue;
}


bool RCX::ChannelGetAttached(uint8_t ch)
{
    if(ch >= RCX_CHANNEL_NUM)
        return false;

    return ChannelReg_Grp[ch] == NULL ? false : true;
}

int16_t** RCX::ChannelGetAttachList()
{
    return ChannelReg_Grp;
}

void RCX::ChannelWrite(uint8_t ch, int16_t value)
{
    if(ch >= RCX_CHANNEL_NUM)
        return;

    int16_t* chGrp = GetPackChannel();
    chGrp[ch] = constrain(value, -RCX_ChannelData_Max, RCX_ChannelData_Max);
}

int16_t RCX::ChannelRead(uint8_t ch)
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
        if(ChannelReg_Grp[i])
        {
            chGrp[i] = constrain(*(ChannelReg_Grp[i]), -RCX_ChannelData_Max, RCX_ChannelData_Max);
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
