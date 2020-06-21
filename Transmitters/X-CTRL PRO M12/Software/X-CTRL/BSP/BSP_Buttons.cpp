#include "Basic/FileGroup.h"
#include "GUI/DisplayPrivate.h"
#include "BSP.h"

ButtonEvent btGrp[BTN_IDX_MAX];

static void ButtonEvent_Handler(ButtonEvent* btn, int event)
{
    if(btn == &btPOWER)
    {
        if(event == ButtonEvent::EVENT_ButtonPress)
        {
            MotorLRA_Vibrate(1, 50);
        }
        if(event == ButtonEvent::EVENT_ButtonLongPressed)
        {
            if(EEPROM_SaveAll())
            {
                MotorLRA_Vibrate(1, 200);
            }                
            Power_Shutdown();
        }
        if(event == ButtonEvent::EVENT_ButtonDoubleClick)
        {
            if(btUPL && btDOWNL)
            {
                Audio_PlayMusic(MC_Type::MC_Astronomia);
            }
        }
    }
    
    uint8_t keyVal;
    if(event == ButtonEvent::EVENT_ButtonPress)
    {
        Audio_Tone(500, 20);//播放操作音(500Hz, 持续20ms)
        keyVal = 1;
    }
    else if(event == ButtonEvent::EVENT_ButtonRelease)
    {
        Audio_Tone(700, 20);//播放操作音(700Hz, 持续20ms)
        keyVal = 0;
    }
    else
        goto PageEvent;
    
    /*对应的CTRL按键标志位*/
    if (btn == &btUP)
        CTRL.Key.Bit.BT_UP = keyVal;
    if (btn == &btDOWN)
        CTRL.Key.Bit.BT_DOWN = keyVal;
    if (btn == &btOK)
        CTRL.Key.Bit.BT_OK = keyVal;
    if (btn == &btBACK)
        CTRL.Key.Bit.BT_BACK = keyVal;
    
    if (btn == &btUPL)
        CTRL.Key.Bit.BT_L1 = keyVal;
    if (btn == &btDOWNL)
        CTRL.Key.Bit.BT_R1 = keyVal;

PageEvent:
    /*传递到页面事件*/
    page.PageEventTransmit(event, btn);
}

void Button_Init()
{
    DEBUG_FUNC_LOG();
    
    /*HC165引脚初始化*/
    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);
    
    __LoopExecute(btGrp[i].EventAttach(ButtonEvent_Handler), __Sizeof(btGrp));
    Switch_Init();
}

typedef enum{
    BTN_L_UP    = 0x0200,
    BTN_L_DOWN  = 0x0100,
    BTN_R_UP1   = 0x0002,
    BTN_R_DOWN1 = 0x0001,
    BTN_R_UP2   = 0x0008,
    BTN_R_DOWN2 = 0x0004,
}ButtonBit_Type;

/**
  * @brief  获取HC165的值
  * @param  无
  * @retval 无
  */
static uint16_t Button_GetHC165Value()
{
    digitalWrite(HC165_CP_Pin, HIGH);
    digitalWrite(HC165_PL_Pin, LOW);
    digitalWrite(HC165_PL_Pin, HIGH);
    uint8_t high = shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
    uint8_t low = ~shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
    return high << 8 | low;
}

void Button_Update()
{
    uint16_t ButtonVal = Button_GetHC165Value();
    
    btUP.EventMonitor(bool(ButtonVal & BTN_R_UP1));
    btDOWN.EventMonitor(bool(ButtonVal & BTN_R_DOWN2));
    btOK.EventMonitor(bool(ButtonVal & BTN_R_DOWN1));
    btBACK.EventMonitor(bool(ButtonVal & BTN_R_UP2));
    btUPL.EventMonitor(bool(ButtonVal & BTN_L_UP));
    btDOWNL.EventMonitor(bool(ButtonVal & BTN_L_DOWN));
    btPOWER.EventMonitor(!digitalRead(CHG_KEY_Pin));
    
    Switch_Update(ButtonVal);
}
