#include "FileGroup.h"

BottonEvent btUP;
BottonEvent btDOWN;
BottonEvent btOK;
BottonEvent btBACK;
BottonEvent btL1;
BottonEvent btL2;
BottonEvent btR1;
BottonEvent btR2;
BottonEvent btEcd(EncoderKey_Pin, 1, 500);

void When_BottonPress()
{
    if(!SilentMode) tone(Buzz_Pin, 500, 20);

    if (btUP)
    {
        CTRL.Key |= BT_UP;
        Menu_Up();
    }
    if (btDOWN)
    {
        CTRL.Key |= BT_DOWN;
        Menu_Down();
    }
    if (btOK)CTRL.Key |= BT_OK;
    if (btBACK)CTRL.Key |= BT_BACK;
    if (btL1)CTRL.Key |= BT_L1;
    if (btR1)CTRL.Key |= BT_R1;
    if (btL2)CTRL.Key |= BT_L2;
    if (btR2)CTRL.Key |= BT_R2;
    if (btEcd) {};
}

void When_BottonLongPress()
{
    if (btUP)
    {
        CTRL.Key |= BT_UP;
        Menu_Up();
    }
    if (btDOWN)
    {
        CTRL.Key |= BT_DOWN;
        Menu_Down();
    }
    if (btOK)CTRL.Key |= BT_OK;
    if (btBACK)CTRL.Key |= BT_BACK;
    if (btL1)CTRL.Key |= BT_L1;
    if (btR1)CTRL.Key |= BT_R1;
    if (btL2)CTRL.Key |= BT_L2;
    if (btR2)CTRL.Key |= BT_R2;
    if (btEcd) {};
}

void When_BottonRelease()
{
	if(!SilentMode) tone(Buzz_Pin, 700, 20);
	
    if (!btUP)CTRL.Key &= ~BT_UP;
    if (!btDOWN)CTRL.Key &= ~BT_DOWN;
    if (!btOK)CTRL.Key &= ~BT_OK;
    if (!btBACK)CTRL.Key &= ~BT_BACK;
    if (!btL1)CTRL.Key &= ~BT_L1;
    if (!btR1)CTRL.Key &= ~BT_R1;
    if (!btL2)CTRL.Key &= ~BT_L2;
    if (!btR2)CTRL.Key &= ~BT_R2;
    if (!btEcd) {};
}

void Init_BottonEvent()
{
    btUP.	EventAttach_Press(When_BottonPress);
    btDOWN.	EventAttach_Press(When_BottonPress);
    btOK.	EventAttach_Press(When_BottonPress);
    btBACK.	EventAttach_Press(When_BottonPress);
    btL1.	EventAttach_Press(When_BottonPress);
    btL2.	EventAttach_Press(When_BottonPress);
    btR1.	EventAttach_Press(When_BottonPress);
    btR2.	EventAttach_Press(When_BottonPress);
    btEcd.	EventAttach_Press(When_BottonPress);

    btUP.	EventAttach_LongPress(When_BottonLongPress);
    btDOWN.	EventAttach_LongPress(When_BottonLongPress);
    btOK.	EventAttach_LongPress(When_BottonLongPress);
    btBACK.	EventAttach_LongPress(When_BottonLongPress);
    btL1.	EventAttach_LongPress(When_BottonLongPress);
    btL2.	EventAttach_LongPress(When_BottonLongPress);
    btR1.	EventAttach_LongPress(When_BottonLongPress);
    btR2.	EventAttach_LongPress(When_BottonLongPress);
    btEcd.	EventAttach_LongPress(When_BottonLongPress);

    btUP.	EventAttach_Release(When_BottonRelease);
    btDOWN.	EventAttach_Release(When_BottonRelease);
    btOK.	EventAttach_Release(When_BottonRelease);
    btBACK.	EventAttach_Release(When_BottonRelease);
    btL1.	EventAttach_Release(When_BottonRelease);
    btL2.	EventAttach_Release(When_BottonRelease);
    btR1.	EventAttach_Release(When_BottonRelease);
    btR2.	EventAttach_Release(When_BottonRelease);
    btEcd.	EventAttach_Release(When_BottonRelease);

    btEcd.Init(INPUT_PULLUP);
}

uint8_t Get_HC165_Value()
{
    digitalWrite_HIGH(HC165_CP_Pin);
    digitalWrite_LOW(HC165_PL_Pin);
    digitalWrite_HIGH(HC165_PL_Pin);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

void BottonEvent_Monitor()
{
    uint8_t BottonValue = Get_HC165_Value();

    btUP.Read_NoWipeShake(bitRead(BottonValue, 4));
    btDOWN.Read_NoWipeShake(bitRead(BottonValue, 7));
    btOK.Read_NoWipeShake(bitRead(BottonValue, 6));
    btBACK.Read_NoWipeShake(bitRead(BottonValue, 5));
    btL1.Read_NoWipeShake(bitRead(BottonValue, 1));
    btL2.Read_NoWipeShake(bitRead(BottonValue, 2));
    btR1.Read_NoWipeShake(bitRead(BottonValue, 0));
    btR2.Read_NoWipeShake(bitRead(BottonValue, 3));
    btEcd.Read_NoWipeShake(digitalRead(EncoderKey_Pin));
}
