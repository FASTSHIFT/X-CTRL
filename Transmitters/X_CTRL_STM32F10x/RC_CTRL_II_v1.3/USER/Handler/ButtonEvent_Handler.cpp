#include "FileGroup.h"

ButtonEvent btUP(1, 500);
ButtonEvent btDOWN(1, 500);
ButtonEvent btOK(1, 500);
ButtonEvent btBACK(1, 500);
ButtonEvent btL1(1, 500);
ButtonEvent btR1(1, 500);
ButtonEvent btL2(1, 500);
ButtonEvent btR2(1, 500);

void When_ButtonPress()
{
    BuzzTone(500, 20);

    if (btUP)
    {
        CTRL.Key |= BT_UP;
    }
    if (btDOWN)
    {
        CTRL.Key |= BT_DOWN;
    }
    if (btOK)
    {
        CTRL.Key |= BT_OK;
    }
    if (btBACK)
    {
        CTRL.Key |= BT_BACK;
    }

    if (btL1)
    {
        CTRL.Key |= BT_L1;
    }
    if (btR1)
    {
        CTRL.Key |= BT_R1;
    }
    if (btL2)
    {
        CTRL.Key |= BT_L2;
    }
    if (btR2)
    {
        CTRL.Key |= BT_R2;
    }

    pm.PageButtonPressEvent();
}

void When_ButtonLongPress()
{
    if (btUP)
    {
        CTRL.Key |= BT_UP;
    }
    if (btDOWN)
    {
        CTRL.Key |= BT_DOWN;
    }
    if (btOK)
    {
        CTRL.Key |= BT_OK;
    }
    if (btBACK)
    {
        CTRL.Key |= BT_BACK;
    }

    if (btL1)
    {
        CTRL.Key |= BT_L1;
    }
    if (btR1)
    {
        CTRL.Key |= BT_R1;
    }
    if (btL2)
    {
        CTRL.Key |= BT_L2;
    }
    if (btR2)
    {
        CTRL.Key |= BT_R2;
    }

    pm.PageButtonLongPressEvent();
}

void When_ButtonRelease()
{
    BuzzTone(700, 20);

    if (!btUP)CTRL.Key &= ~BT_UP;
    if (!btDOWN)CTRL.Key &= ~BT_DOWN;
    if (!btOK)CTRL.Key &= ~BT_OK;
    if (!btBACK)CTRL.Key &= ~BT_BACK;
    if (!btL1)CTRL.Key &= ~BT_L1;
    if (!btR1)CTRL.Key &= ~BT_R1;
    if (!btL2)CTRL.Key &= ~BT_L2;
    if (!btR2)CTRL.Key &= ~BT_R2;
}

void Init_Button()
{
    pinMode(Button_OK_Pin, INPUT_PULLUP);
    pinMode(Button_BACK_Pin, INPUT_PULLUP);
    pinMode(Button_UP_Pin, INPUT_PULLUP);
    pinMode(Button_DOWN_Pin, INPUT_PULLUP);
    pinMode(Button_L_Pin, INPUT_PULLUP);
    pinMode(Button_R_Pin, INPUT_PULLUP);

    if(ConnectState.Pattern != Pattern_NRF)
    {
        pinMode(Button_JSL_Pin, INPUT_PULLUP);
        pinMode(Button_JSR_Pin, INPUT_PULLUP);
    }

    btUP.	EventAttach_Press(When_ButtonPress);
    btDOWN.	EventAttach_Press(When_ButtonPress);
    btOK.	EventAttach_Press(When_ButtonPress);
    btBACK.	EventAttach_Press(When_ButtonPress);
    btL1.	EventAttach_Press(When_ButtonPress);
    btL2.	EventAttach_Press(When_ButtonPress);
    btR1.	EventAttach_Press(When_ButtonPress);
    btR2.	EventAttach_Press(When_ButtonPress);

    btUP.	EventAttach_LongPress(When_ButtonLongPress);
    btDOWN.	EventAttach_LongPress(When_ButtonLongPress);
    btOK.	EventAttach_LongPress(When_ButtonLongPress);
    btBACK.	EventAttach_LongPress(When_ButtonLongPress);
    btL1.	EventAttach_LongPress(When_ButtonLongPress);
    btL2.	EventAttach_LongPress(When_ButtonLongPress);
    btR1.	EventAttach_LongPress(When_ButtonLongPress);
    btR2.	EventAttach_LongPress(When_ButtonLongPress);

    btUP.	EventAttach_Release(When_ButtonRelease);
    btDOWN.	EventAttach_Release(When_ButtonRelease);
    btOK.	EventAttach_Release(When_ButtonRelease);
    btBACK.	EventAttach_Release(When_ButtonRelease);
    btL1.	EventAttach_Release(When_ButtonRelease);
    btL2.	EventAttach_Release(When_ButtonRelease);
    btR1.	EventAttach_Release(When_ButtonRelease);
    btR2.	EventAttach_Release(When_ButtonRelease);
}

void ButtonEvent_Monitor()
{
    btUP.Read_NoWipeShake(digitalRead(Button_UP_Pin));
    btDOWN.Read_NoWipeShake(digitalRead(Button_DOWN_Pin));
    btOK.Read_NoWipeShake(digitalRead(Button_OK_Pin));
    btBACK.Read_NoWipeShake(digitalRead(Button_BACK_Pin));
    btL2.Read_NoWipeShake(digitalRead(Button_L_Pin));
    btR2.Read_NoWipeShake(digitalRead(Button_R_Pin));

    if(ConnectState.Pattern != Pattern_NRF)
    {
        btL1.Read_NoWipeShake(digitalRead(Button_JSL_Pin));
        btR1.Read_NoWipeShake(digitalRead(Button_JSR_Pin));
    }
}
