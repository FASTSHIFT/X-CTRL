#include "FileGroup.h"
#include "ComPrivate.h"

int16_t pwm = 500;

static void NormalCtrlState()
{
    pwm = map(RC_X.Left.Y,-CtrlOutput_MaxValue, CtrlOutput_MaxValue, 0, 1000);
}

static void LostCtrlState()
{
    pwm = 500;
}

void Task_Ctrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
        NormalCtrlState();
    else
        LostCtrlState();
    
    analogWrite(CH1_Pin, pwm);
    analogWrite(CH2_Pin, pwm);
    analogWrite(CH3_Pin, pwm);
    analogWrite(CH4_Pin, pwm);
    analogWrite(CH5_Pin, pwm);
    analogWrite(CH6_Pin, pwm);
}
