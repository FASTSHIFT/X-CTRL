#include "BSP.h"

static bool State_MotorVibrate = true;
static uint32_t MotorStop_TimePoint = 0;
static bool IsMotorRunning = false;

void Motor_Init()
{
    PWM_Init(Motor_IN1_Pin, 1000, 20000);
    PWM_Init(Motor_IN2_Pin, 1000, 20000);
}

void Motor_WriteData(int16_t data)
{
    pwmWrite(Motor_IN1_Pin, data >= 0 ? data : 0);
    pwmWrite(Motor_IN2_Pin, data < 0 ? -data : 0);
}

void Motor_Update()
{
    if(IsMotorRunning && millis() >= MotorStop_TimePoint)
    {
        Motor_NoTone();
        IsMotorRunning = false;
    }
}

void Motor_SetEnable(bool en)
{
    State_MotorVibrate = en;
    if(!en)Motor_NoTone();
}

void Motor_Vibrate(float strength, uint32_t time)
{
    if(!State_MotorVibrate)
        return;

    __LimitValue(strength, 0.0f, 1.0f);

    Motor_ToneSetVolume(strength * 800);
    Motor_Tone(100);
    
    IsMotorRunning = true;
    MotorStop_TimePoint = millis() + time;
}

#include "arm_math.h"

#define MOTOR_TONE_SAMPLE_FREQ   50000
#define MOTOR_TONE_HZ_TO_US(hz) (1000000.0f/(hz))
#define MOTOR_TONE_TIM           TIM_MOTOR_TONE
#define MOTOR_TONE_MAX_VAL       1000

static const uint32_t Tone_SamplePeriod = MOTOR_TONE_HZ_TO_US(MOTOR_TONE_SAMPLE_FREQ);
static uint32_t Tone_SampleTick;
static uint32_t Tone_StopTime;
static bool Tone_AutoStop = false;
static float Tone_SignalPeriod;
static uint16_t Tone_Volume;

void Motor_ToneSetVolume(uint16_t volume)
{
    if(volume > MOTOR_TONE_MAX_VAL)
        volume = MOTOR_TONE_MAX_VAL;
    
    Tone_Volume = volume;
}

uint16_t Motor_ToneGetVolume()
{
    return Tone_Volume;
}

void Motor_NoTone()
{
    TIM_Cmd(MOTOR_TONE_TIM, DISABLE);
    Motor_WriteData(0);
}

static void Motor_ToneTimerCallback()
{
    if(Tone_AutoStop && millis() >= Tone_StopTime)
    {
        Motor_NoTone();
        return;
    }

    float fraction = (Tone_SampleTick * Tone_SamplePeriod) / Tone_SignalPeriod;
    fraction -= (int)fraction;
    int16_t output = arm_sin_q15(fraction * 32767);
    output *= (1000.0f / 32767.0f);
    Motor_WriteData(output * ((float)Tone_Volume / MOTOR_TONE_MAX_VAL));
    Tone_SampleTick++;
}

static void Motor_ToneInit()
{
    Timer_SetInterruptBase(MOTOR_TONE_TIM, 0xFF, 0xFF, Motor_ToneTimerCallback, 0, 1);
    Timer_SetInterruptFreqUpdate(MOTOR_TONE_TIM, MOTOR_TONE_SAMPLE_FREQ);
}

void Motor_Tone(float freq)
{
    __ExecuteOnce(Motor_ToneInit());
    
    if(freq <= 1 || freq > 20000)
    {
        Motor_NoTone();
        return;
    }

    Tone_AutoStop = false;
    
    uint32_t us = MOTOR_TONE_HZ_TO_US(freq);
    if(us != Tone_SignalPeriod)
    {
        Tone_SampleTick = 0;
        Tone_SignalPeriod = us;
    }
    TIM_Cmd(MOTOR_TONE_TIM, ENABLE);
}

void Motor_Tone(float freq, uint32_t time)
{
    if(freq <= 1 || freq > 20000 || time == 0)
    {
        Motor_NoTone();
        return;
    }
    Motor_Tone(freq);
    Tone_AutoStop = true;
    Tone_StopTime = millis() + time;
}
