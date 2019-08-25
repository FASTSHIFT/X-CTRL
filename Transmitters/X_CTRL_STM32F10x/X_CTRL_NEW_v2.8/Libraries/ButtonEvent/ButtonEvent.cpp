#include "ButtonEvent.h"

#define HAVE_MILLIS

#ifdef HAVE_MILLIS
#include "Arduino.h"
#define ButtonEvent_Millis millis()
#else
#define ButtonEvent_Millis
#warning "Please set the system clock used by ButtonEvent"
#endif

/**
  * @brief  按键事件构造函数
  * @param  NoPressState: 按键未按下时的状态
  * @param  LongPressTimeMs_Set: 按键长按触发超时设置
  * @retval 无
  */
ButtonEvent::ButtonEvent(bool NoPressState, uint16_t LongPressTimeMs_Set)
{
    Button_NoPressState = NoPressState;
    LongPressTimeMs = LongPressTimeMs_Set;

    PressKey_TimePoint = 0;
    IS_Pressed = false;

    ButtonPress_Function = 0;
    ButtonLongPress_Function = 0;
    ButtonRelease_Function = 0;
    ButtonChange_Function = 0;
}

/**
  * @brief  按键按下事件绑定
  * @param  Function: 回调函数指针
  * @retval 无
  */
void ButtonEvent::EventAttach_Press(ButtonEvent_FuncCallBack_t Function)
{
    ButtonPress_Function = Function;
}

/**
  * @brief  按键长按事件绑定
  * @param  Function: 回调函数指针
  * @retval 无
  */
void ButtonEvent::EventAttach_LongPress(ButtonEvent_FuncCallBack_t Function)
{
    ButtonLongPress_Function = Function;
}

/**
  * @brief  按键释放事件绑定
  * @param  Function: 回调函数指针
  * @retval 无
  */
void ButtonEvent::EventAttach_Release(ButtonEvent_FuncCallBack_t Function)
{
    ButtonRelease_Function = Function;
}

/**
  * @brief  按键状态改变事件绑定
  * @param  Function: 回调函数指针
  * @retval 无
  */
void ButtonEvent::EventAttach_Change(ButtonEvent_FuncCallBack_t Function)
{
    ButtonChange_Function = Function;
}

/**
  * @brief  监控事件，建议扫描周期10ms
  * @param  NowState: 当前按键引脚状态
  * @retval 无
  */
void ButtonEvent::EventMonitor(uint8_t NowState)
{
    if (!IS_Pressed && (NowState != Button_NoPressState))
    {
        IS_Pressed = true;
        Button_NowState = Press;
        PressKey_TimePoint = ButtonEvent_Millis + LongPressTimeMs;
        if(ButtonPress_Function) ButtonPress_Function();
        if(ButtonChange_Function)ButtonChange_Function();
    }
    else if (IS_Pressed && ButtonEvent_Millis >= PressKey_TimePoint && (NowState != Button_NoPressState))
    {
        Button_NowState = LongPress;
        if(ButtonLongPress_Function) ButtonLongPress_Function();
    }
    else if (IS_Pressed && (NowState == Button_NoPressState))
    {
        IS_Pressed = false;
        Button_NowState = NoPress;
        if(ButtonRelease_Function) ButtonRelease_Function();
        if(ButtonChange_Function) ButtonChange_Function();
    }
}

