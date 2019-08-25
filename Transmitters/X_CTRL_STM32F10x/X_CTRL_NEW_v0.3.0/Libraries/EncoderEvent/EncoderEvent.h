#ifndef __ENCODEREVENT_H
#define __ENCODEREVENT_H

#include "Arduino.h"

typedef void(*EncoderEvent_FuncCallBack_t)(void);

class EncoderEvent{
public:
	EncoderEvent(uint8_t encoderA_Pin,uint8_t encoderB_Pin);
	void Init(uint8_t PinMode = INPUT);
	void EventAttach_Clockwise(EncoderEvent_FuncCallBack_t Function);
	void EventAttach_Anticlockwise(EncoderEvent_FuncCallBack_t Function);
	int8_t Read();
	operator int8_t() { return Encoder_NowState;};
private:
	uint8_t EncoderA_Pin, EncoderB_Pin;
	int8_t Encoder_NowState;
	bool encoderALast;

	EncoderEvent_FuncCallBack_t Clockwise_Function;
	EncoderEvent_FuncCallBack_t Anticlockwise_Function;
};

#endif
