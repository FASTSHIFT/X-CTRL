#include "FileGroup.h"
#include "PageX_Manager.h"

BottonEvent btUP;
BottonEvent btDOWN;
BottonEvent btLEFT;
BottonEvent btRIGHT;
BottonEvent btA;
BottonEvent btB;
BottonEvent btL1;
BottonEvent btR1;

uint8_t BottonState;

void When_Botton_Press()
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
        CTRL.Key |= BT_OK;
    }
    if (btLEFT)
    {
        CTRL.Key |= BT_BACK;
    }

    if (btL1)CTRL.Key |= BT_L1;
    if (btR1)CTRL.Key |= BT_R1;
    if (btA)CTRL.Key |= BT_A;
    if (btB)CTRL.Key |= BT_B;

    BT_print = BottonState;

    PageBottonPressEvent_x();
}

void When_Botton_LongPress()
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
        CTRL.Key |= BT_OK;
    }
    if (btLEFT)
    {
        CTRL.Key |= BT_BACK;
    }

    if (btL1)CTRL.Key |= BT_L1;
    if (btR1)CTRL.Key |= BT_R1;
    if (btA)CTRL.Key |= BT_A;
    if (btB)CTRL.Key |= BT_B;

    PageBottonLongPressEvent_x();
}

void When_BottonRelease()
{
    if (!btUP)CTRL.Key &= ~BT_UP;
    if (!btDOWN)CTRL.Key &= ~BT_DOWN;
    if (!btRIGHT)CTRL.Key &= ~BT_OK;
    if (!btLEFT)CTRL.Key &= ~BT_BACK;
    if (!btL1)CTRL.Key &= ~BT_L1;
    if (!btR1)CTRL.Key &= ~BT_R1;
    if (!btA)CTRL.Key &= ~BT_A;
    if (!btB)CTRL.Key &= ~BT_B;
}

uint8_t Get_HC165_Value()
{
    digitalWrite(HC165_CP_Pin, HIGH);
    digitalWrite(HC165_PL_Pin, LOW);
    digitalWrite(HC165_PL_Pin, HIGH);
    return shiftIn(HC165_OUT_Pin, HC165_CP_Pin, MSBFIRST);
}

void BottonEvent_Init()
{
    btUP.EventAttach_Press(When_Botton_Press);
    btUP.EventAttach_LongPress(When_Botton_LongPress);
    btUP.EventAttach_Release(When_BottonRelease);

    btDOWN.EventAttach_Press(When_Botton_Press);
    btDOWN.EventAttach_LongPress(When_Botton_LongPress);
    btDOWN.EventAttach_Release(When_BottonRelease);

    btLEFT.EventAttach_Press(When_Botton_Press);
    btLEFT.EventAttach_LongPress(When_Botton_LongPress);
    btLEFT.EventAttach_Release(When_BottonRelease);

    btRIGHT.EventAttach_Press(When_Botton_Press);
    btRIGHT.EventAttach_LongPress(When_Botton_LongPress);
    btRIGHT.EventAttach_Release(When_BottonRelease);

    btA.EventAttach_Press(When_Botton_Press);
    btA.EventAttach_LongPress(When_Botton_LongPress);
    btA.EventAttach_Release(When_BottonRelease);

    btB.EventAttach_Press(When_Botton_Press);
    btB.EventAttach_LongPress(When_Botton_LongPress);
    btB.EventAttach_Release(When_BottonRelease);

    btL1.EventAttach_Press(When_Botton_Press);
    btL1.EventAttach_LongPress(When_Botton_LongPress);
    btL1.EventAttach_Release(When_BottonRelease);

    btR1.EventAttach_Press(When_Botton_Press);
    btR1.EventAttach_LongPress(When_Botton_LongPress);
    btR1.EventAttach_Release(When_BottonRelease);
}

void BottonEvent_Monitor()
{
    BottonState = Get_HC165_Value();

    btUP.Read_NoWipeShake(BottonState & BT_UP);
    btDOWN.Read_NoWipeShake(BottonState & BT_DOWN);
    btLEFT.Read_NoWipeShake(BottonState & BT_LEFT);
    btRIGHT.Read_NoWipeShake(BottonState & BT_RIGHT);
    btA.Read_NoWipeShake(BottonState & BT_A);
    btB.Read_NoWipeShake(BottonState & BT_B);
    btL1.Read_NoWipeShake(BottonState & BT_L1);
    btR1.Read_NoWipeShake(BottonState & BT_R1);
}

	


