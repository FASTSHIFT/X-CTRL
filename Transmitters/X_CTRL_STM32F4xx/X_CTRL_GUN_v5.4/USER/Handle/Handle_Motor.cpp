#include "FileGroup.h"
#include "MusicPlayer.h"

bool State_MotorVibrate = true;
static uint32_t MotorStop_TimePoint = 0;
static bool IsMotorRunning = false;

void Init_Motor()
{
    pinMode(Motor_Pin, PWM);
    analogWrite(Motor_Pin, 0);
}

void Task_MotorRunning()
{
    if(IsMotorRunning && millis()>= MotorStop_TimePoint)
    {
        analogWrite(Motor_Pin, 0);
        IsMotorRunning = false;
    }
}

void MotorVibrate(float strength, uint32_t time)
{
   if(!State_MotorVibrate)
       return;
   
   __LimitValue(strength, 0.0f, 1.0f);
   
   analogWrite(Motor_Pin, strength * 1000);
   IsMotorRunning = true;
   MotorStop_TimePoint = millis() + time;
}
