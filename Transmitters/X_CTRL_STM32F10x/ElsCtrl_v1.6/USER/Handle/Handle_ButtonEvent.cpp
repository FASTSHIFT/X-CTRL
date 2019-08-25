#include "FileGroup.h"
#include "GUI_Private.h"

ButtonEvent btUP(1, 500);
ButtonEvent btDOWN(1, 500);
ButtonEvent btOK(1, 500);

void When_ButtonPress()
{
    digitalWrite_HIGH(InfoLED_Pin);
    BuzzTone(500, 20);

    if (btUP == btUP.Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == btDOWN.Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == btOK.Press)
        CTRL.Key |= BT_OK;

    page.PageEventTransmit(EVENT_ButtonPress);
}

void When_ButtonLongPress()
{
    page.PageEventTransmit(EVENT_ButtonLongPress);
}

void When_ButtonRelease()
{
    digitalWrite_LOW(InfoLED_Pin);
    BuzzTone(700, 20);

    if (btUP == btUP.NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == btDOWN.NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == btOK.NoPress)
        CTRL.Key &= ~BT_OK;
}

void Init_BottonEvent()
{
    pinMode(Button_OK_Pin, INPUT_PULLUP);
    pinMode(Button_UP_Pin, INPUT_PULLUP);
    pinMode(Button_DOWN_Pin, INPUT_PULLUP);
    
    btUP.	EventAttach_Press(When_ButtonPress);
    btDOWN.	EventAttach_Press(When_ButtonPress);
    btOK.	EventAttach_Press(When_ButtonPress);

    btUP.	EventAttach_LongPress(When_ButtonLongPress);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress);
    btOK.	EventAttach_LongPress(When_ButtonLongPress);

    btUP.	EventAttach_Release(When_ButtonRelease);
    btDOWN.	EventAttach_Release(When_ButtonRelease);
    btOK.	EventAttach_Release(When_ButtonRelease);
}

void ButtonEventMonitor()
{  
    btUP.   EventMonitor(digitalRead(Button_UP_Pin));
    btDOWN. EventMonitor(digitalRead(Button_DOWN_Pin));
    btOK.   EventMonitor(digitalRead(Button_OK_Pin));
}
