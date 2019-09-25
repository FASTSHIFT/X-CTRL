#include "FileGroup.h"
#include "TasksManage.h"

TimerHandle_t TimerHandle_Motor;

bool State_MotorVibrate = true;
static uint32_t MotorStop_TimePoint = 0;
static bool IsMotorRunning = false;

static void Init_Motor()
{
    pinMode(Speaker_Pin, OUTPUT);
    //digitalWrite(Speaker_Pin, LOW);
    
    PWM_Init(Motor_Pin, 1000, 1000);
    analogWrite(Motor_Pin, 0);
}

void Task_MotorRunning(TimerHandle_t xTimer)
{
    static bool isInit = false;
    if(!isInit)
    {
        Init_Motor();
        isInit = true;
    }
    
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
