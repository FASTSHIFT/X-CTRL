#include "FileGroup.h"
#include "GUI_Private.h"
#include "ButtonPrivate.h"

using namespace Key;

ButtonEvent btGrp[MAX];

void When_ButtonPress()
{
    BuzzTone(500, 20);

    page.PageButtonPressEvent();
}

void When_ButtonLongPress()
{
    page.PageButtonLongPressEvent();
}

void When_ButtonRelease()
{
    BuzzTone(700, 20);
}

uint8_t Get_HC165_Value()
{
    digitalWrite_HIGH(HC165_CP_Pin);
    digitalWrite_LOW(HC165_PL_Pin);
    digitalWrite_HIGH(HC165_PL_Pin);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

void Init_BottonEvent()
{
    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);
    
    pinMode(Button_UP_Pin,INPUT_PULLDOWN);
    pinMode(Button_DOWN_Pin,INPUT_PULLDOWN);
    pinMode(Button_LEFT_Pin,INPUT_PULLDOWN);
    pinMode(Button_RIGHT_Pin,INPUT_PULLDOWN);
    pinMode(Button_POWER_Pin,INPUT_PULLDOWN);
    pinMode(Button_SHIFT_Pin,INPUT_PULLDOWN);
    
    for(uint8_t bt = 0; bt<MAX ; bt++)
    {
        btGrp[bt].EventAttach_Press(When_ButtonPress);
        btGrp[bt].EventAttach_LongPress(When_ButtonLongPress);
        btGrp[bt].EventAttach_Release(When_ButtonRelease);
    }
}

void ButtonEventMonitor()
{
    uint8_t ButtonValue = Get_HC165_Value();
    btGrp[Up].EventMonitor(digitalRead(Button_UP_Pin));
    btGrp[Down].EventMonitor(digitalRead(Button_DOWN_Pin));
    btGrp[Left].EventMonitor(digitalRead(Button_LEFT_Pin));
    btGrp[Right].EventMonitor(digitalRead(Button_RIGHT_Pin));
    btGrp[Power].EventMonitor(digitalRead(Button_POWER_Pin));
    btGrp[Shift].EventMonitor(digitalRead(Button_SHIFT_Pin));
    
    btGrp[Bt1].EventMonitor(bitRead(ButtonValue,2));
    btGrp[Bt2].EventMonitor(bitRead(ButtonValue,1));
    btGrp[Bt3].EventMonitor(bitRead(ButtonValue,0));
    btGrp[Bt4].EventMonitor(bitRead(ButtonValue,6));
    btGrp[Bt5].EventMonitor(bitRead(ButtonValue,7));
    btGrp[Bt6].EventMonitor(bitRead(ButtonValue,3));
}
