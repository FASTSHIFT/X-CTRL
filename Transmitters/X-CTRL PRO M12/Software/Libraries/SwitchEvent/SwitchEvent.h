#ifndef __SWITCHEVENT_H
#define __SWITCHEVENT_H

#include "stdint.h"

class SwitchEvent{
    typedef void(*SwitchEvent_FuncCallBack_t)(SwitchEvent*,int);
public:
    enum Event_Type
    {
        EVENT_SwitchNone = 10,
        EVENT_SwitchUp,
        EVENT_SwitchDown,
        EVENT_SwitchOff,
    };
    SwitchEvent();
    void EventAttach(SwitchEvent_FuncCallBack_t func);
    void EventMonitor(bool isup, bool isdown);
    bool Is_UP;
    bool Is_DOWN;
    bool Is_OFF;

    operator int8_t()
    {
        return NowState;
    };
private:
    SwitchEvent_FuncCallBack_t EventCallFunc;
    int8_t NowState, LastState;
};

#endif
