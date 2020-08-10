#include "BSP.h"

static bool MotorLRA_Vibrate_Enable = true;
static uint32_t MotorLRA_StopTimePoint = 0;
static bool MotorLRA_IsRunning = false;

void MotorLRA_Init()
{
    PWM_Init(MotorLRA_IN1_Pin, 1000, 20000);
    PWM_Init(MotorLRA_IN2_Pin, 1000, 20000);
}

void MotorLRA_WriteData(int16_t data)
{
    pwmWrite(MotorLRA_IN1_Pin, data >= 0 ? data : 0);
    pwmWrite(MotorLRA_IN2_Pin, data < 0 ? -data : 0);
}

void MotorLRA_Update()
{
    if(MotorLRA_IsRunning && millis() >= MotorLRA_StopTimePoint)
    {
        MotorLRA_NoTone();
        MotorLRA_IsRunning = false;
    }
}

void MotorLRA_SetEnable(bool en)
{
    MotorLRA_Vibrate_Enable = en;
    if(!en)MotorLRA_NoTone();
}

void MotorLRA_Vibrate(float strength, uint32_t time)
{
    if(!MotorLRA_Vibrate_Enable)
        return;

    __LimitValue(strength, 0.0f, 1.0f);

    MotorLRA_ToneSetVolume(strength * 800);
    MotorLRA_Tone(100);
    
    MotorLRA_IsRunning = true;
    MotorLRA_StopTimePoint = millis() + time;
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
static uint16_t Tone_Volume = MOTOR_TONE_MAX_VAL;

void MotorLRA_ToneSetVolume(uint16_t volume)
{
    if(volume > MOTOR_TONE_MAX_VAL)
        volume = MOTOR_TONE_MAX_VAL;
    
    Tone_Volume = volume;
}

uint16_t MotorLRA_ToneGetVolume()
{
    return Tone_Volume;
}

void MotorLRA_NoTone()
{
    TIM_Cmd(MOTOR_TONE_TIM, DISABLE);
    MotorLRA_WriteData(0);
}

static void MotorLRA_ToneTimerCallback()
{
    if(Tone_AutoStop && millis() >= Tone_StopTime)
    {
        MotorLRA_NoTone();
        return;
    }

    float fraction = (Tone_SampleTick * Tone_SamplePeriod) / Tone_SignalPeriod;
    fraction -= (int)fraction;
    int16_t output = arm_sin_q15(fraction * 32767);
    output *= (1000.0f / 32767.0f);
    MotorLRA_WriteData(output * ((float)Tone_Volume / MOTOR_TONE_MAX_VAL));
    Tone_SampleTick++;
}

static void MotorLRA_ToneInit()
{
    Timer_SetInterruptBase(MOTOR_TONE_TIM, 0xFF, 0xFF, MotorLRA_ToneTimerCallback, 0, 1);
    Timer_SetInterruptFreqUpdate(MOTOR_TONE_TIM, MOTOR_TONE_SAMPLE_FREQ);
}

void MotorLRA_Tone(float freq)
{
    __ExecuteOnce(MotorLRA_ToneInit());
    
    if(freq <= 1 || freq > 20000)
    {
        MotorLRA_NoTone();
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

void MotorLRA_Tone(float freq, uint32_t time)
{
    if(freq <= 1 || freq > 20000 || time == 0)
    {
        MotorLRA_NoTone();
        return;
    }
    MotorLRA_Tone(freq);
    Tone_AutoStop = true;
    Tone_StopTime = millis() + time;
}
