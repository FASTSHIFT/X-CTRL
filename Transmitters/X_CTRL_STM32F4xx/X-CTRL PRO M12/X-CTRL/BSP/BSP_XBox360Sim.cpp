#include "BSP.h"
#include "Communication/ComPrivate.h"

#define BigVibrateThreshold 180

void Passback_XBox360Sim(uint8_t state)
{
    static uint16_t motorToneVolume = 0;
    
    if(state == PBS_Setup)
    {
        BigMotor_SetEnable(true);
        motorToneVolume = Motor_ToneGetVolume();
    }
    else if(state == PBS_Loop)
    {
        /*BIG*/
        int16_t motorLeft = RCX::GetRxPackChannel(2);

        /*small*/
        int16_t motorRight = RCX::GetRxPackChannel(3);

        int16_t motorPWM = motorLeft + motorRight * 0.7f;

        static uint16_t motorPWM_Max = 100;
        __IntervalExecute(motorPWM_Max = 100, 20 * 1000);

        if(motorPWM > motorPWM_Max)
        {
            motorPWM_Max = motorPWM;
        }

        Motor_Vibrate(
            fmap(
                motorPWM,
                0, motorPWM_Max,
                motorPWM == 0 ? 0 : 0.25f, 1.0f
            ),
            1000
        );

        static int16_t BigMotorVal = 0;
        int16_t BigMotorVal_Target = 0;
        if(motorPWM > BigVibrateThreshold)
        {
            BigMotorVal_Target = map(motorPWM, BigVibrateThreshold, motorPWM_Max, 0, 100);
            if(BigMotorVal < BigMotorVal_Target)
            {
                BigMotorVal++;
            }
        }
        else
        {
            if(BigMotorVal > BigMotorVal_Target)
            {
                BigMotorVal--;
            }
        }
        BigMotor_SetValue(BigMotorVal);
    }
    else if(state == PBS_Error)
    {
        BigMotor_SetValue(0);
    }
    else if(state == PBS_Exit)
    {
        BigMotor_SetEnable(false);
        Motor_Tone(0);
        Motor_ToneSetVolume(motorToneVolume);
    }
}
