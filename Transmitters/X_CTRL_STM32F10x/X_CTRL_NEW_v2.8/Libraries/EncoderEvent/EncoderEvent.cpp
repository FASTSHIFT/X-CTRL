#include "EncoderEvent.h"

//说明：旋转编码器事件构造函数
//参数：A引脚编号；B引脚编号
EncoderEvent::EncoderEvent(uint8_t encoderA_Pin,uint8_t encoderB_Pin)
{
    EncoderA_Pin = encoderA_Pin;
	EncoderB_Pin = encoderB_Pin;
	Encoder_NowState = 0;
	encoderALast = false;
	
    Clockwise_Function = 0;
    Anticlockwise_Function = 0;
}

void EncoderEvent::Init(uint8_t PinMode)
{
	pinMode(EncoderA_Pin,PinMode);
	pinMode(EncoderB_Pin,PinMode);
} 

//说明：旋转编码器顺时针事件绑定
//参数：回调函数指针
void EncoderEvent::EventAttach_Clockwise(EncoderEvent_FuncCallBack_t Function)
{
    Clockwise_Function = Function;
}

//说明：旋转编码器逆时针事件绑定
//参数：回调函数指针
void EncoderEvent::EventAttach_Anticlockwise(EncoderEvent_FuncCallBack_t Function)
{
    Anticlockwise_Function = Function;
}


//说明：读取旋转编码器
//参数：无
int8_t EncoderEvent::Read()
{
    bool encoderA = digitalRead(EncoderA_Pin);
    if ((encoderALast == true) && (encoderA == false))
    {
        if (digitalRead(EncoderB_Pin) == false)
        {
			Encoder_NowState = 1;
            if(Clockwise_Function) Clockwise_Function();
        }
        else
        {
			Encoder_NowState = -1;
            if(Anticlockwise_Function) Anticlockwise_Function();
        }
    }
	else Encoder_NowState = 0;
    encoderALast = encoderA;
	
	return Encoder_NowState;
}
