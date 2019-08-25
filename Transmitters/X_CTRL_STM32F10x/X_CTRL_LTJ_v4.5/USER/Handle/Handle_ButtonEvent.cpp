#include "FileGroup.h"
#include "GUI_Private.h"

ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btOK;
ButtonEvent btBACK;
ButtonEvent btL1;
ButtonEvent btL2;
ButtonEvent btR1;
ButtonEvent btR2;
ButtonEvent btEcd(1, 500);
ButtonEvent swSPDT;

void When_ButtonPress()
{
    BuzzTone(500, 20);

    if (btUP == btUP.Press)
        CTRL.Key |= BT_UP;
    if (btDOWN == btDOWN.Press)
        CTRL.Key |= BT_DOWN;
    if (btOK == btOK.Press)
        CTRL.Key |= BT_OK;
    if (btBACK == btBACK.Press)
        CTRL.Key |= BT_BACK;
    if (btL1 == btL1.Press)
        CTRL.Key |= BT_L1;
    if (btR1 == btR1.Press)
        CTRL.Key |= BT_R1;
    if (btL2 == btL2.Press)
        CTRL.Key |= BT_L2;
    if (btR2 == btR2.Press)
        CTRL.Key |= BT_R2;

    page.PageEventTransmit(EVENT_ButtonPress);
}

void When_ButtonLongPress()
{
    page.PageEventTransmit(EVENT_ButtonLongPress);
}

void When_ButtonRelease()
{
    BuzzTone(700, 20);

    if (btUP == btUP.NoPress)
        CTRL.Key &= ~BT_UP;
    if (btDOWN == btDOWN.NoPress)
        CTRL.Key &= ~BT_DOWN;
    if (btOK == btOK.NoPress)
        CTRL.Key &= ~BT_OK;
    if (btBACK == btBACK.NoPress)
        CTRL.Key &= ~BT_BACK;
    if (btL1 == btL1.NoPress)
        CTRL.Key &= ~BT_L1;
    if (btR1 == btR1.NoPress)
        CTRL.Key &= ~BT_R1;
    if (btL2 == btL2.NoPress)
        CTRL.Key &= ~BT_L2;
    if (btR2 == btR2.NoPress)
        CTRL.Key &= ~BT_R2;
}

void When_ChangeSPDT()
{
    if(digitalRead(SPDT_Switch_Pin))
    {
        BuzzTone(500, 300);
        XFS_Clear();
        XFS_Speak("Ò£¿ØÆ÷½âËø");
    }
    else
    {
        BuzzTone(200, 100);
        XFS_Clear();
        XFS_Speak("Ò£¿ØÆ÷Ëø¶¨");
    }
}

void Init_BottonEvent()
{
    btUP.	EventAttach_Press(When_ButtonPress);
    btDOWN.	EventAttach_Press(When_ButtonPress);
    btOK.	EventAttach_Press(When_ButtonPress);
    btBACK.	EventAttach_Press(When_ButtonPress);
    btL1.	EventAttach_Press(When_ButtonPress);
    btL2.	EventAttach_Press(When_ButtonPress);
    btR1.	EventAttach_Press(When_ButtonPress);
    btR2.	EventAttach_Press(When_ButtonPress);
    btEcd.	EventAttach_Press(When_ButtonPress);

    btUP.	EventAttach_LongPress(When_ButtonLongPress);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress);
    btOK.	EventAttach_LongPress(When_ButtonLongPress);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress);
    btL1.	EventAttach_LongPress(When_ButtonLongPress);
    btL2.	EventAttach_LongPress(When_ButtonLongPress);
    btR1.	EventAttach_LongPress(When_ButtonLongPress);
    btR2.	EventAttach_LongPress(When_ButtonLongPress);
    btEcd.	EventAttach_LongPress(When_ButtonLongPress);

    btUP.	EventAttach_Release(When_ButtonRelease);
    btDOWN.	EventAttach_Release(When_ButtonRelease);
    btOK.	EventAttach_Release(When_ButtonRelease);
    btBACK.	EventAttach_Release(When_ButtonRelease);
    btL1.	EventAttach_Release(When_ButtonRelease);
    btL2.	EventAttach_Release(When_ButtonRelease);
    btR1.	EventAttach_Release(When_ButtonRelease);
    btR2.	EventAttach_Release(When_ButtonRelease);
    btEcd.	EventAttach_Release(When_ButtonRelease);

    pinMode(EncoderKey_Pin, INPUT_PULLUP);
    swSPDT.EventAttach_Change(When_ChangeSPDT);
}

uint8_t Get_HC165_Value()
{
    digitalWrite_HIGH(HC165_CP_Pin);
    digitalWrite_LOW(HC165_PL_Pin);
    digitalWrite_HIGH(HC165_PL_Pin);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}
uint8_t ButtonValue;
void ButtonEventMonitor()
{
    ButtonValue = Get_HC165_Value();

    btUP.   EventMonitor(!bitRead(ButtonValue, 4));
    btDOWN. EventMonitor(!bitRead(ButtonValue, 7));
    btOK.   EventMonitor(!bitRead(ButtonValue, 6));
    btBACK. EventMonitor(!bitRead(ButtonValue, 5));
    btL1.   EventMonitor(bitRead(ButtonValue, 3));
    btL2.   EventMonitor(!bitRead(ButtonValue, 0));
    btR1.   EventMonitor(bitRead(ButtonValue, 2));
    btR2.   EventMonitor(!bitRead(ButtonValue, 1));
//    btEcd.  EventMonitor(digitalRead(EncoderKey_Pin));
//    swSPDT. EventMonitor(digitalRead(SPDT_Switch_Pin));
}
