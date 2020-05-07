#include "Basic/FileGroup.h"
#include "ComPrivate.h"
#include "BSP/IMU_Private.h"

typedef struct{
    int16_t*   pDevVal;
    const char* DevName;
}DevMapChannel_TypeDef;

static const DevMapChannel_TypeDef DevMapChannel_Grp[] = 
{
    {NULL, "---"},
    {&CTRL.JS_L.X.Val, "CTRL.JS_L.X"},
    {&CTRL.JS_L.Y.Val, "CTRL.JS_L.Y"},
    {&CTRL.JS_R.X.Val, "CTRL.JS_R.X"},
    {&CTRL.JS_R.Y.Val, "CTRL.JS_R.Y"},
    
    {&CTRL.Switch[SW_IDX_E], "CTRL.Switch[E]"},
    {&CTRL.Switch[SW_IDX_F], "CTRL.Switch[F]"},
    {&CTRL.Switch[SW_IDX_G], "CTRL.Switch[G]"},
    {&CTRL.Switch[SW_IDX_H], "CTRL.Switch[H]"},
    
    {&IMU_Channel.Pitch.Data, "IMU.Pitch"},
    {&IMU_Channel.Roll.Data,  "IMU.Roll"},
    {&IMU_Channel.Yaw.Data,   "IMU.Yaw"},
    
    {&IMU_Channel.Pitch.delta, "IMU.Pitch.delta"},
    {&IMU_Channel.Roll.delta,  "IMU.Roll.delta"},
    {&IMU_Channel.Yaw.delta,   "IMU.Yaw.delta"},
};

int16_t* Com_DevGetPtr(int16_t index)
{
    int16_t* retval = NULL;
    if(index > 0 && index <__Sizeof(DevMapChannel_Grp))
    {
        retval = DevMapChannel_Grp[index].pDevVal;
    }
    return retval;
}

const char* Com_DevGetName(int16_t index)
{
    const char* retval = DevMapChannel_Grp[0].DevName;
    if(index > 0 && index <__Sizeof(DevMapChannel_Grp))
    {
        retval = DevMapChannel_Grp[index].DevName;
    }
    return retval;
}

int16_t Com_DevGetIndex(int16_t* pValue)
{
    for(int i = 0; i < __Sizeof(DevMapChannel_Grp); i++)
    {
        if(pValue != NULL && DevMapChannel_Grp[i].pDevVal == pValue)
        {
            return i;
        }
    }
    return -1;
}

void Com_DevChannelUpdate()
{
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        int16_t* pValue = RCX::ChannelGetAttached(i);
        int16_t index = Com_DevGetIndex(pValue);
        if(index < 0) index = 0;
        CTRL.CH_AttachMap[i] = index;
    }
}

void Com_ChannelInit()
{
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        int index = CTRL.CH_AttachMap[i];
        if(index < __Sizeof(DevMapChannel_Grp))
        {
            RCX::ChannelSetAttach(i, DevMapChannel_Grp[index].pDevVal);
        }
        
        RCX::ChannelSetReverse(i, CTRL.CH_Reverse[i]);
    }
}

/**
  * @brief  初始化默认通道配置
  * @param  无
  * @retval 无
  */
void Com_ChannelSetDefault()
{
    RCX::ChannelSetAttachEnable(true);
    
    RCX::ChannelSetAttach(0, &CTRL.Switch[SW_IDX_E]);
    RCX::ChannelSetAttach(1, &CTRL.Switch[SW_IDX_F]);
    RCX::ChannelSetAttach(2, &CTRL.Switch[SW_IDX_G]);
    RCX::ChannelSetAttach(3, &CTRL.Switch[SW_IDX_H]);
    RCX::ChannelSetAttach(4, &CTRL.JS_L.X.Val);
    RCX::ChannelSetAttach(5, &CTRL.JS_L.Y.Val);
    RCX::ChannelSetAttach(6, &CTRL.JS_R.X.Val);
    RCX::ChannelSetAttach(7, &CTRL.JS_R.Y.Val);
    Com_DevChannelUpdate();
    
    for(int i = 0; i < RCX_CHANNEL_NUM; i++)
    {
        CTRL.CH_Reverse[i] = false;
        RCX::ChannelSetReverse(i, CTRL.CH_Reverse[i]);
    }
}
