#include "Basic/FileGroup.h"
#include "BSP.h"

static bool BM_Enable = false;

static const uint16_t BM_TIM_OverFlow = 99;
static const uint16_t BM_TIM_FreqHz = 500;

static uint32_t BM_TIM_CNT = 0;
static uint16_t BM_TIM_CCR = 0;

static void BigMotor_TimerCallback()
{
    BM_TIM_CNT++;
    (BM_TIM_CNT < BM_TIM_CCR) ? digitalWrite_HIGH(BigMotor_Pin) : digitalWrite_LOW(BigMotor_Pin);
    
    if(BM_TIM_CNT >= BM_TIM_OverFlow)
    {
        BM_TIM_CNT = 0;
    }
}

void BigMotor_Init()
{
    pinMode(BigMotor_Pin, OUTPUT);
    digitalWrite(BigMotor_Pin, LOW);
    Timer_SetInterruptBase(TIM_BIG_MOTOR, 0xFF, 0xFF, BigMotor_TimerCallback, 0, 2);
    Timer_SetInterruptFreqUpdate(TIM_BIG_MOTOR, BM_TIM_FreqHz * (BM_TIM_OverFlow + 1));
    BigMotor_SetEnable(false);
}

void BigMotor_SetEnable(bool en)
{
    BM_Enable = en;
    if(en)
    {
        TIM_Cmd(TIM_BIG_MOTOR, ENABLE);
        BM_TIM_CNT = 0;
    }
    else
    {
        TIM_Cmd(TIM_BIG_MOTOR, DISABLE);
        digitalWrite(BigMotor_Pin, LOW);
    }
}

bool BigMotor_GetEnable()
{
    return BM_Enable;
}

void BigMotor_SetValue(uint16_t value)
{
    if(!CTRL.State.Vibrate && !BM_Enable)
        return;
    
    if(value > BM_TIM_OverFlow + 1)
    {
        value = BM_TIM_OverFlow + 1;
    }
    
    BM_TIM_CCR = value;
}
