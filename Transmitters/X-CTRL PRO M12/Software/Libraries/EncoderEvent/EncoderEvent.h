#ifndef __ENCODEREVENT_H
#define __ENCODEREVENT_H

#include "stdint.h"

class EncoderEvent
{
    typedef void(*EncoderEvent_FuncCallBack_t)(EncoderEvent*, int);
public:
    enum Event_Type
    {
        EVENT_None = 10,
        EVENT_Clockwise,
        EVENT_Anticlockwise
    };
    EncoderEvent();
    void EventAttach(EncoderEvent_FuncCallBack_t Function);
    void EventMonitor(bool NowState_A, bool NowState_B);
    operator int8_t()
    {
        return Encoder_NowState;
    };
private:
    int8_t Encoder_NowState;
    bool encoderALast;

    EncoderEvent_FuncCallBack_t CallbackFunc;
};

#endif
