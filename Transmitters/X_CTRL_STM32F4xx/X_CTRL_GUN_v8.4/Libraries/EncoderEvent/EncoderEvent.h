#ifndef __ENCODEREVENT_H
#define __ENCODEREVENT_H

#include "stdint.h"

class EncoderEvent{
    typedef void(*EncoderEvent_FuncCallBack_t)(void);
public:
	EncoderEvent();
	void EventAttach_Rotate(EncoderEvent_FuncCallBack_t Function);
	int8_t EventMonitor(bool NowState_A, bool NowState_B);
	operator int8_t() { return Encoder_NowState;};
private:
	int8_t Encoder_NowState;
	bool encoderALast;

	EncoderEvent_FuncCallBack_t Rotate_Function;
};

#endif
