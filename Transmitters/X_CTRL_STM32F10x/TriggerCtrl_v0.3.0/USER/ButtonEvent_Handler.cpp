#include "FileGroup.h"

ButtonEvent btUP;
ButtonEvent btDOWN;
ButtonEvent btLEFT;
ButtonEvent btRIGHT;
ButtonEvent btA;
ButtonEvent btB;
ButtonEvent btL1;
ButtonEvent btR1;

void When_Button_Press()
{
    tone(Buzz_Pin, 600, 20);
    if (btUP)
    {
        CTRL.Key |= BT_UP;
    }
    if (btDOWN)
    {
        CTRL.Key |= BT_DOWN;
    }
    if (btRIGHT)
    {
        CTRL.Key |= BT_RIGHT;
    }
    if (btLEFT)
    {
        CTRL.Key |= BT_LEFT;
    }

    if (btL1)CTRL.Key |= BT_L2;
    if (btR1)CTRL.Key |= BT_R2;
    if (btA)CTRL.Key |= BT_L1;
    if (btB)CTRL.Key |= BT_R1;

    pm.PageButtonPressEvent();
}

void When_Button_LongPress()
{
    if (btUP)
    {
        CTRL.Key |= BT_UP;
    }
    if (btDOWN)
    {
        CTRL.Key |= BT_DOWN;
    }
    if (btRIGHT)
    {
        CTRL.Key |= BT_RIGHT;
    }
    if (btLEFT)
    {
        CTRL.Key |= BT_LEFT;
    }

    if (btL1)CTRL.Key |= BT_L2;
    if (btR1)CTRL.Key |= BT_R2;
    if (btA)CTRL.Key |= BT_L1;
    if (btB)CTRL.Key |= BT_R1;

    pm.PageButtonLongPressEvent();
}

void When_ButtonRelease()
{
    if (!btUP)CTRL.Key &= ~BT_UP;
    if (!btDOWN)CTRL.Key &= ~BT_DOWN;
    if (!btRIGHT)CTRL.Key &= ~BT_RIGHT;
    if (!btLEFT)CTRL.Key &= ~BT_LEFT;
    if (!btL1)CTRL.Key &= ~BT_L2;
    if (!btR1)CTRL.Key &= ~BT_R2;
    if (!btA)CTRL.Key &= ~BT_L1;
    if (!btB)CTRL.Key &= ~BT_R1;
}

uint8_t Get_HC165_Value()
{
    digitalWrite_HIGH(HC165_CP_Pin);
    digitalWrite_LOW(HC165_PL_Pin);
    digitalWrite_HIGH(HC165_PL_Pin);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

void Init_ButtonEvent()
{
    btUP.EventAttach_Press(When_Button_Press);
    btUP.EventAttach_LongPress(When_Button_LongPress);
    btUP.EventAttach_Release(When_ButtonRelease);

    btDOWN.EventAttach_Press(When_Button_Press);
    btDOWN.EventAttach_LongPress(When_Button_LongPress);
    btDOWN.EventAttach_Release(When_ButtonRelease);

    btLEFT.EventAttach_Press(When_Button_Press);
    btLEFT.EventAttach_LongPress(When_Button_LongPress);
    btLEFT.EventAttach_Release(When_ButtonRelease);

    btRIGHT.EventAttach_Press(When_Button_Press);
    btRIGHT.EventAttach_LongPress(When_Button_LongPress);
    btRIGHT.EventAttach_Release(When_ButtonRelease);

    btA.EventAttach_Press(When_Button_Press);
    btA.EventAttach_LongPress(When_Button_LongPress);
    btA.EventAttach_Release(When_ButtonRelease);

    btB.EventAttach_Press(When_Button_Press);
    btB.EventAttach_LongPress(When_Button_LongPress);
    btB.EventAttach_Release(When_ButtonRelease);

    btL1.EventAttach_Press(When_Button_Press);
    btL1.EventAttach_LongPress(When_Button_LongPress);
    btL1.EventAttach_Release(When_ButtonRelease);

    btR1.EventAttach_Press(When_Button_Press);
    btR1.EventAttach_LongPress(When_Button_LongPress);
    btR1.EventAttach_Release(When_ButtonRelease);
}

void ButtonEvent_Monitor()
{
    enum {
        _UP		= B00000100,
        _DOWN	= B00001000,
        _LEFT	= B00000001,
        _RIGHT	= B00000010,
        _B		= B00100000,
        _A		= B00010000,
        _L1 	= B01000000,
        _R1 	= B10000000
    };
    uint8_t ButtonState = Get_HC165_Value();

    btUP.Read_NoWipeShake(ButtonState & _UP);
    btDOWN.Read_NoWipeShake(ButtonState & _DOWN);
    btLEFT.Read_NoWipeShake(ButtonState & _LEFT);
    btRIGHT.Read_NoWipeShake(ButtonState & _RIGHT);
    btA.Read_NoWipeShake(ButtonState & _A);
    btB.Read_NoWipeShake(ButtonState & _B);
    btL1.Read_NoWipeShake(ButtonState & _L1);
    btR1.Read_NoWipeShake(ButtonState & _R1);
}
