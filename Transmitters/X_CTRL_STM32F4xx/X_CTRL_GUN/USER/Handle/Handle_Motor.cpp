#include "FileGroup.h"

static uint32_t MotorStop_TimePoint = 0;
static bool IsMotorRunning = false;

static void Init_Motor()
{
    PWM_Init(Motor_Pin, 1000, 500);
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
   if(!CTRL.State.Vibrate)
       return;
   
   __ExecuteOnce(Init_Motor());
   
   __LimitValue(strength, 0.0f, 1.0f);
   
   analogWrite(Motor_Pin, strength * 1000);
   IsMotorRunning = true;
   MotorStop_TimePoint = millis() + time;
}
