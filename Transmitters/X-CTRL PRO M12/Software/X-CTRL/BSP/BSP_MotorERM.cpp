#include "Basic/FileGroup.h"
#include "BSP.h"

static const uint16_t ERM_TIM_OverFlow = 99;
static const uint16_t ERM_TIM_FreqHz = 500;

static uint32_t ERM_TIM_CNT = 0;
static uint16_t ERM_TIM_CCR = 0;

static void MotorERM_TimerCallback()
{
    ERM_TIM_CNT++;
    (ERM_TIM_CNT < ERM_TIM_CCR) ? digitalWrite_HIGH(MotorERM_Pin) : digitalWrite_LOW(MotorERM_Pin);
    
    if(ERM_TIM_CNT >= ERM_TIM_OverFlow)
    {
        ERM_TIM_CNT = 0;
    }
}

void MotorERM_Init()
{
    pinMode(MotorERM_Pin, OUTPUT);
    digitalWrite(MotorERM_Pin, LOW);
    Timer_SetInterruptBase(TIM_BIG_MOTOR, 0xFF, 0xFF, MotorERM_TimerCallback, 0, 2);
    Timer_SetInterruptFreqUpdate(TIM_BIG_MOTOR, ERM_TIM_FreqHz * (ERM_TIM_OverFlow + 1));
    MotorERM_SetEnable(false);
}

void MotorERM_SetEnable(bool en)
{
    if(en && CTRL.State->ERM_Vibrate)
    {
        TIM_Cmd(TIM_BIG_MOTOR, ENABLE);
        ERM_TIM_CNT = 0;
    }
    else
    {
        TIM_Cmd(TIM_BIG_MOTOR, DISABLE);
        digitalWrite(MotorERM_Pin, LOW);
    }
}

void MotorERM_SetValue(uint16_t value)
{
    if(!CTRL.State->ERM_Vibrate)
        return;
    
    if(value > ERM_TIM_OverFlow + 1)
    {
        value = ERM_TIM_OverFlow + 1;
    }
    
    ERM_TIM_CCR = value;
}
