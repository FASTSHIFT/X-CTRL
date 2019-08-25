#include "FileGroup.h"

void Task_MID_Setting()
{

}

void Task_CarCtrl()
{
    int16_t MotorSpeed_L, MotorSpeed_R;
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        MotorSpeed_L = DS_CAR.MotorSpeed.L;
        MotorSpeed_R = DS_CAR.MotorSpeed.R;
    }
    else
    {
        MotorSpeed_L = 0;
        MotorSpeed_R = 0;
    }

    if(MotorSpeed_L > 5)
    {
        digitalWrite(MotorL_1_Pin, HIGH);
        digitalWrite(MotorL_2_Pin, LOW);
    }
    else if(MotorSpeed_L < -5)
    {
        digitalWrite(MotorL_1_Pin, LOW);
        digitalWrite(MotorL_2_Pin, HIGH);
    }
    else
    {
        digitalWrite(MotorL_1_Pin, LOW);
        digitalWrite(MotorL_2_Pin, LOW);
    }

    if(MotorSpeed_R > 5)
    {
        digitalWrite(MotorR_1_Pin, HIGH);
        digitalWrite(MotorR_2_Pin, LOW);
    }
    else if(MotorSpeed_R < -5)
    {
        digitalWrite(MotorR_1_Pin, LOW);
        digitalWrite(MotorR_2_Pin, HIGH);
    }
    else
    {
        digitalWrite(MotorR_1_Pin, LOW);
        digitalWrite(MotorR_2_Pin, LOW);
    }

    analogWrite(MotorL_Pin, ABS(MotorSpeed_L));
    analogWrite(MotorR_Pin, ABS(MotorSpeed_R));
}

void Task_BuzzRing_Block()
{
    togglePin(PA4);
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
