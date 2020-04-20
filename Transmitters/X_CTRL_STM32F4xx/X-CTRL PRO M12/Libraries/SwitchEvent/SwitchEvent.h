#ifndef __SWITCHEVENT_H
#define __SWITCHEVENT_H

#include "stdint.h"

namespace SwitchEvent_Type
{
typedef enum
{
    EVENT_SwitchNone = 10,
    EVENT_SwitchUp,
    EVENT_SwitchDown,
    EVENT_SwitchOff,
} Event_Type;
};

class SwitchEvent{
    typedef void(*SwitchEvent_FuncCallBack_t)(SwitchEvent*,int);
public:
    SwitchEvent();
    void EventAttach(SwitchEvent_FuncCallBack_t func);
    void EventMonitor(bool isup, bool isdown);
    bool Is_UP;
    bool Is_DOWN;
    bool Is_OFF;
private:
    SwitchEvent_FuncCallBack_t EventCallFunc;
    int8_t NowState, LastState;
};

#endif
