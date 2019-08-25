#include "FileGroup.h"

#define DET_TO_SPEED        0.1473f

float ElsSpeed = 0;
float ElsDistance = 0;
uint32_t SpdDet_Cnt = 0;
uint32_t TotalDet = 0;

static void SpeedDet_Callback()
{
    if(IsLowPowerEnable)
        NVIC_SystemReset();

    SpdDet_Cnt++;
}

static void Task_CalcSpeed()
{
    TotalDet += SpdDet_Cnt;
    ElsDistance = TotalDet * DET_TO_SPEED / 3.6 / 10;
    ElsSpeed = SpdDet_Cnt * DET_TO_SPEED;
    SpdDet_Cnt = 0;
}

void Init_SpeedDet()
{
    pinMode(SPD_DET_Pin, INPUT);
    attachInterrupt(SPD_DET_Pin, SpeedDet_Callback, CHANGE);
    TimerSet(TIM_CalcSpeed, 100 * 1000, Task_CalcSpeed);
    TIM_Cmd(TIM_CalcSpeed, ENABLE);
}


