#include "FileGroup.h"
#include "ComPrivate.h"

#define DEAD_ZONE_VAL 40

static void MotorWrite(int16_t val)
{
    analogWrite(M1_IN1_Pin, val > DEAD_ZONE_VAL ? val : 0);
    analogWrite(M1_IN2_Pin, val < -DEAD_ZONE_VAL ? -val : 0);
}

static void SteerWrite(int16_t val)
{
    analogWrite(M2_IN1_Pin, val > DEAD_ZONE_VAL ? val : 0);
    analogWrite(M2_IN2_Pin, val < -DEAD_ZONE_VAL ? -val : 0);
}

void Task_Ctrl()
{
    if(!RCX::GetRxConnected())
    {
        MotorWrite(0);
        SteerWrite(0);
        return;
    }
    
    int16_t motorVal = map(RCX::GetRxPackChannel(CH_JSL_Y), -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX, -1000, 1000);
    motorVal *= (float)RCX::GetRxPackChannel(CH_LMT_L) / RCX_CHANNEL_DATA_MAX;
    MotorWrite(motorVal);
    
    int16_t steerVal = map(RCX::GetRxPackChannel(CH_JSR_X), -RCX_CHANNEL_DATA_MAX, RCX_CHANNEL_DATA_MAX, -1000, 1000);
    steerVal *= (float)RCX::GetRxPackChannel(CH_LMT_R) / RCX_CHANNEL_DATA_MAX;
    SteerWrite(steerVal);
}
