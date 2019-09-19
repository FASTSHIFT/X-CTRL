#include "FileGroup.h"

ButtonEvent btUP(HIGH);
ButtonEvent btDOWN(HIGH);

static void ButtonPressEvent_Handler()
{
    if(btUP && btDOWN)
    {
        ResolutionRatio_Select++;
        ResolutionRatio_Select %= SIZE(ResolutionRatio);
        return;
    }
    
    if(btUP)
    {
        if(MaxTemp < 80)
            MaxTemp += 2;
    }
    if(btDOWN)
    {
        if(MinTemp < MaxTemp)
            MinTemp += 2;
    }
}

static void ButtonLongPressEvent_Handler()
{   
    if(btUP)
    {
        if(MaxTemp > MinTemp)
            MaxTemp -= 2;
    }
    if(btDOWN)
    {
        if(MinTemp > 0)
            MinTemp -= 2;
    }
}

void Init_ButtonEvent()
{
    pinMode(Button_UP_Pin, INPUT_PULLUP);
    pinMode(Button_Down_Pin, INPUT_PULLUP);

    btUP.EventAttach_Press(ButtonPressEvent_Handler);
    btDOWN.EventAttach_Press(ButtonPressEvent_Handler);

    btUP.EventAttach_LongPress(ButtonLongPressEvent_Handler);
    btDOWN.EventAttach_LongPress(ButtonLongPressEvent_Handler);
}

void Task_ButtonEventMonitor()
{
    btUP.EventMonitor(digitalRead(Button_UP_Pin));
    btDOWN.EventMonitor(digitalRead(Button_Down_Pin));
}
