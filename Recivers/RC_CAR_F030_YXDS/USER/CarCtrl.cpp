#include "FileGroup.h"

Servo AngleServo;
void Task_MID_Setting()
{

}

void Task_CarCtrl()
{
    uint16_t MotorSpeed_L, MotorSpeed_R;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        MotorSpeed_L = map(DS_CAR.MotorSpeed.L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 50, 950);
        MotorSpeed_R = map(DS_CAR.MotorSpeed.R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue, 50, 950);
    }
    else
    {
        MotorSpeed_L = MotorSpeed_MID;
        MotorSpeed_R = MotorSpeed_MID;
    }
    analogWrite(MotorL_Pin, MotorSpeed_L);
    analogWrite(MotorR_Pin, MotorSpeed_R);
}

void Task_BuzzRing_Block()
{
    enum {Sound_Start = 1, Sound_Connect, Sound_Disonnect, Sound_Error};
    static uint8_t Sound_Now, Sound_Last;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Sound_Connect;
        if(Sound_Now != Sound_Last)
        {
            toneBlock(Buzz_Pin, 1046, 80);
            toneBlock(Buzz_Pin, 1175, 80);
            toneBlock(Buzz_Pin, 1318, 80);
            Sound_Last = Sound_Now;
        }
    }
    else if(!ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        Sound_Now = Sound_Disonnect;
        if(Sound_Now != Sound_Last)
        {
            toneBlock(Buzz_Pin, 100, 80);
            delay(80);
            toneBlock(Buzz_Pin, 100, 80);
            Sound_Last = Sound_Now;
        }
    }
}
