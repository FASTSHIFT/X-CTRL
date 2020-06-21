#include "SwitchEvent.h"

SwitchEvent::SwitchEvent()
{
    NowState = LastState = 0;
}

void SwitchEvent::EventAttach(SwitchEvent_FuncCallBack_t func)
{
    EventCallFunc = func;
}

void SwitchEvent::EventMonitor(bool isup, bool isdown)
{
    Is_UP = isup;
    Is_DOWN = isdown;
    Is_OFF = !(Is_UP | Is_DOWN);
    
    if(isup)
    {
        NowState = 1;
    }
    else if(isdown)
    {
        NowState = -1;
    }
    else
    {
        NowState = 0;
    }
    
    if(!EventCallFunc)
        return;
    
    if(NowState != LastState)
    {
        if(isup)
        {
            EventCallFunc(this, EVENT_SwitchUp);
        }
        else if(isdown)
        {
            EventCallFunc(this, EVENT_SwitchDown);
        }
        else
        {
            EventCallFunc(this, EVENT_SwitchOff);
        }
        LastState = NowState;
    }
}
