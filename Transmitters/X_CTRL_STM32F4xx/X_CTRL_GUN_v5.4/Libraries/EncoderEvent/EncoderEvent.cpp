#include "EncoderEvent.h"

//说明：旋转编码器事件构造函数
EncoderEvent::EncoderEvent()
{
	Encoder_NowState = 0;
	encoderALast = false;
    Rotate_Function = 0;
}

//说明：旋转编码器事件绑定
//参数：回调函数指针
void EncoderEvent::EventAttach_Rotate(EncoderEvent_FuncCallBack_t Function)
{
    Rotate_Function = Function;
}


//说明：读取旋转编码器
//参数：A相状态，B相状态
int8_t EncoderEvent::EventMonitor(bool NowState_A, bool NowState_B)
{
    if ((encoderALast == true) && (NowState_A == false))
    {
        if (NowState_B == false)
			Encoder_NowState = 1;
        else
			Encoder_NowState = -1;

        if(Rotate_Function)
            Rotate_Function();
    }
	else 
        Encoder_NowState = 0;
    encoderALast = NowState_A;
	
	return Encoder_NowState;
}
