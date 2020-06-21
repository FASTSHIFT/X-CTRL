#include "EncoderEvent.h"

/**
  * @brief  旋转编码器事件构造函数
  * @param  无
  * @retval 无
  */
EncoderEvent::EncoderEvent()
{
    Encoder_NowState = 0;
    encoderALast = false;
    CallbackFunc = 0;
}

/**
  * @brief  旋转编码器事件绑定
  * @param  Function: 回调函数指针
  * @retval 无
  */
void EncoderEvent::EventAttach(EncoderEvent_FuncCallBack_t Function)
{
    CallbackFunc = Function;
}

/**
  * @brief  监控旋转编码器事件
  * @param  NowState_A: A相状态
  * @param  NowState_B: B相状态
  * @retval 无
  */
void EncoderEvent::EventMonitor(bool NowState_A, bool NowState_B)
{
    if(CallbackFunc == 0)
        return;

    if ((encoderALast == true) && (NowState_A == false))
    {
        if (NowState_B == false)
        {
            Encoder_NowState = 1;
            CallbackFunc(this, EVENT_Clockwise);
        }
        else
        {
            Encoder_NowState = -1;
            CallbackFunc(this, EVENT_Anticlockwise);
        }
    }
    else
    {
        Encoder_NowState = 0;
    }

    encoderALast = NowState_A;
}
