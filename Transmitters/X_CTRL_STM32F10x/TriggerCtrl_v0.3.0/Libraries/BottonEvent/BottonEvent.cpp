#include "BottonEvent.h"

//说明：按键事件构造函数
//参数：引脚编号；按键释放时的状态；长按超时设置
BottonEvent::BottonEvent(uint8_t Pin, bool NoPressState, uint16_t LongPressTimeMs_Set)
{
    Botton_Pin = Pin;
    Botton_NoPressState = NoPressState;
    LongPressTimeMs = LongPressTimeMs_Set;

    PressKey_TimePoint = 0;
    IS_Pressed = false;

    BottonPress_Function = 0;
    BottonLongPress_Function = 0;
    BottonRelease_Function = 0;
    BottonChange_Function = 0;
}

BottonEvent::BottonEvent(bool NoPressState, uint16_t LongPressTimeMs_Set)
{
    Botton_NoPressState = NoPressState;
    LongPressTimeMs = LongPressTimeMs_Set;

    PressKey_TimePoint = 0;
    IS_Pressed = false;

    BottonPress_Function = 0;
    BottonLongPress_Function = 0;
    BottonRelease_Function = 0;
    BottonChange_Function = 0;
}

//说明：按键初始化函数
//参数：按键所接IO模式配置
void BottonEvent::Init(uint8_t PinMode)
{
    pinMode(Botton_Pin, PinMode);
}

//说明：按键按下事件绑定
//参数：回调函数指针
void BottonEvent::EventAttach_Press(BottonEvent_FuncCallBack_t Function)
{
    BottonPress_Function = Function;
}

//说明：按键长按事件绑定
//参数：回调函数指针
void BottonEvent::EventAttach_LongPress(BottonEvent_FuncCallBack_t Function)
{
    BottonLongPress_Function = Function;
}

//说明：按键释放事件绑定
//参数：回调函数指针
void BottonEvent::EventAttach_Release(BottonEvent_FuncCallBack_t Function)
{
    BottonRelease_Function = Function;
}

//说明：按键改变事件绑定
//参数：回调函数指针
void BottonEvent::EventAttach_Change(BottonEvent_FuncCallBack_t Function)
{
    BottonChange_Function = Function;
}

//说明：用软件消抖的方式读取按键
//参数：无
void BottonEvent::Read_WipeShake()
{
#define IS_BottonDown() (digitalRead(Botton_Pin) != Botton_NoPressState)
    if (!IS_Pressed && IS_BottonDown())
    {
        delay(10);
        if (IS_BottonDown())
        {
            Botton_NowState = IS_Pressed = true;
            PressKey_TimePoint = millis() + LongPressTimeMs;
            if(BottonPress_Function)BottonPress_Function();
            if(BottonChange_Function)BottonChange_Function();
        }
    }
    else if (IS_Pressed && millis() >= PressKey_TimePoint && IS_BottonDown())
    {
        Botton_NowState = true;
        if(BottonLongPress_Function)BottonLongPress_Function();
    }
    else if (IS_Pressed && !IS_BottonDown())
    {
        delay(10);
        if (!IS_BottonDown())
        {
            Botton_NowState = IS_Pressed = false;
            if(BottonRelease_Function)BottonRelease_Function();
            if(BottonChange_Function)BottonChange_Function();
        }
    }
}

//说明：直接读取按键
//参数：按键IO的状态
void BottonEvent::Read_NoWipeShake(uint8_t NowState)
{
    if (!IS_Pressed && (NowState != Botton_NoPressState))
    {
        Botton_NowState = IS_Pressed = true;
        PressKey_TimePoint = millis() + LongPressTimeMs;
        if(BottonPress_Function) BottonPress_Function();
        if(BottonChange_Function)BottonChange_Function();
    }
    else if (IS_Pressed && millis() >= PressKey_TimePoint && (NowState != Botton_NoPressState))
    {
        Botton_NowState = true;
        if(BottonLongPress_Function) BottonLongPress_Function();
    }
    else if (IS_Pressed && (NowState == Botton_NoPressState))
    {
        Botton_NowState = IS_Pressed = false;
        if(BottonRelease_Function) BottonRelease_Function();
        if(BottonChange_Function) BottonChange_Function();
    }
}

//说明：重映射引脚
//参数：引脚编号，IO输入模式
void BottonEvent::Remap_Pin(uint8_t Pin, uint8_t PinMode)
{
    Botton_Pin = Pin;
    pinMode(Botton_Pin, PinMode);
}

//说明：获取按键状态
//参数：无
bool BottonEvent::GetState()
{
    return Botton_NowState;
}

//说明：清除按键状态
//参数：无
void BottonEvent::ClearState()
{
    Botton_NowState = false;
}
