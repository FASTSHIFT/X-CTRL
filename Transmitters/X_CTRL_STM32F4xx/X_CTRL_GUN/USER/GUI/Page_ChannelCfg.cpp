#include "FileGroup.h"
#include "DisplayPrivate.h"
#include "ComPrivate.h"
#include "IMU_Private.h"

static MillisTaskManager mtmChannelCfg(2);

static uint8_t ItemSelect = 0;
static bool ItemSelectLock = false;
    

typedef struct{
    int16_t* pVal;
    const char* Name;
    int8_t AttachChannel;
}ChannelDisp_TypeDef;
#define ChannelDispDef(val) {&(val), #val}

ChannelDisp_TypeDef ChannelDisp_Grp[] = {
    {&CTRL.KnobLimit.L, "CTRL.M.L", -1},
    {&CTRL.KnobLimit.R, "CTRL.M.R", -1},
    
    {&CTRL.JS_L.X.Val, "CTRL.L.X", -1},
    {&CTRL.JS_L.Y.Val, "CTRL.L.Y", -1},
    {&CTRL.JS_R.X.Val, "CTRL.R.X", -1},
    {&CTRL.JS_R.Y.Val, "CTRL.R.Y", -1},

    {&IMU_Channel.Pitch.Data, "IMU.Pit ", -1},
    {&IMU_Channel.Roll.Data,  "IMU.Rol ", -1},
    {&IMU_Channel.Yaw.Data,   "IMU.Yaw ", -1},
    
    {&IMU_Channel.Pitch.delta, "IMU.dPit", -1},
    {&IMU_Channel.Roll.delta,  "IMU.dRol", -1},
    {&IMU_Channel.Yaw.delta,   "IMU.dYaw", -1},
};

static void Task_UpdateChannelDisp()
{
    screen.setTextWrap(false);
    for(int i = 0;i < __Sizeof(ChannelDisp_Grp); i++)
    {
        TextSetDefault();
        
        int16_t disp_y = StatusBar_Height + 5 + TEXT_HEIGHT_1 * i;
        
        screen.setTextColor(ItemSelect == i ? screen.Yellow : screen.White, screen.Black);
        screen.setCursor(2, disp_y);
        screen.printf("%s:% 5d->", ChannelDisp_Grp[i].Name, *ChannelDisp_Grp[i].pVal);
        
        if(ItemSelect == i && ItemSelectLock)
            screen.setTextColor(screen.Red, screen.Black);
        
        screen.setCursor(screen.width() - 4 - TEXT_WIDTH_1 * 4, disp_y);
        ChannelDisp_Grp[i].AttachChannel != -1 
        ? screen.printf("CH%d",ChannelDisp_Grp[i].AttachChannel)
        : screen.print("---");
    }
}

static void AttachChannelUpdate()
{
    for(int i = 0;i < __Sizeof(ChannelDisp_Grp); i++)
    {
        ChannelDisp_Grp[i].AttachChannel = RCX::ChannelGetAttachedIndex(ChannelDisp_Grp[i].pVal);
    }
}

/**
  * @brief  页面初始化事件
  * @param  无
  * @retval 无
  */
static void Setup()
{
    ClearPage();
    AttachChannelUpdate();
    mtmChannelCfg.TaskRegister(0, Task_UpdateChannelDisp, 80);
}

/**
  * @brief  页面循环事件
  * @param  无
  * @retval 无
  */
static void Loop()
{
    mtmChannelCfg.Running(millis());
}

/**
  * @brief  页面退出事件
  * @param  无
  * @retval 无
  */
static void Exit()
{
    ClearPage();
}

/**
  * @brief  页面事件
  * @param  无
  * @retval 无
  */
static void Event(int event, void* param)
{
    if(event == EVENT_ButtonPress || event == EVENT_ButtonLongPressRepeat)
    {
        if(!ItemSelectLock)
        {
            if(param == &btUP)
            {
                __ValueStep(ItemSelect, +1, (int)__Sizeof(ChannelDisp_Grp));
            }
            else if(param == &btDOWN)
            {
                __ValueStep(ItemSelect, -1, (int)__Sizeof(ChannelDisp_Grp));
            }
        }
        else
        {
            if(param == &btUP)
            {
                ChannelDisp_Grp[ItemSelect].AttachChannel++;
                if(ChannelDisp_Grp[ItemSelect].AttachChannel >= RCX_CHANNEL_NUM)
                {
                    ChannelDisp_Grp[ItemSelect].AttachChannel = -1;
                }
            }
            else if(param == &btDOWN)
            {
                ChannelDisp_Grp[ItemSelect].AttachChannel--;
                if(ChannelDisp_Grp[ItemSelect].AttachChannel < -1)
                {
                    ChannelDisp_Grp[ItemSelect].AttachChannel = RCX_CHANNEL_NUM - 1;
                }
            }
        }
    }
    
    if(param == &btOK)
    {
        if(event == EVENT_ButtonPress)
        {
            ItemSelectLock = !ItemSelectLock;
            
            if(!ItemSelectLock)
            {
                if(ChannelDisp_Grp[ItemSelect].AttachChannel != -1)
                {
                    RCX::ChannelSetAttachEnable(true);
                    RCX::ChannelSetAttach(
                        ChannelDisp_Grp[ItemSelect].AttachChannel,
                        ChannelDisp_Grp[ItemSelect].pVal
                    );
                }
                else
                {
                    int16_t ch = RCX::ChannelGetAttachedIndex(ChannelDisp_Grp[ItemSelect].pVal);
                    if(ch >= 0)
                        RCX::ChannelSetAttach(ch, NULL);
                }
                
                AttachChannelUpdate();
            }
        }
    }
    
    
    if(param == &btBACK)
    {
        if(event == EVENT_ButtonPress)
        {
            if(ItemSelectLock)
            {
                page.PageEventTransmit(EVENT_ButtonPress, &btOK);
            }
        }
        if(event == EVENT_ButtonLongPressed)
        {
            page.PagePop();
        }
        if(event == EVENT_ButtonDoubleClick)
        {
            if(!RCX::ChannelGetAttachEnable())
            {
                Com_SetDefaultChannel();
            }
            else
            {
                RCX::ChannelSetAttachEnable(false);
            }
            AttachChannelUpdate();
        }
    }
}

/**
  * @brief  页面注册
  * @param  pageID:为此页面分配的ID号
  * @retval 无
  */
void PageRegister_ChannelCfg(uint8_t pageID)
{
    page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
