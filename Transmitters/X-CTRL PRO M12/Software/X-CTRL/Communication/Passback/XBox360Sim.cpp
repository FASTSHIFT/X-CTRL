#include "BSP/BSP.h"
#include "Communication/ComPrivate.h"

static const int16_t BigVibrateThreshold = 180;

void Passback_XBox360Sim(uint8_t state)
{
    static uint16_t motorToneVolume = 0;
    
    if(state == PBS_Setup)
    {
        MotorERM_SetEnable(true);
        motorToneVolume = MotorLRA_ToneGetVolume();
    }
    else if(state == PBS_Loop)
    {
        /*BIG*/
        int16_t motorLeft = map(
            RCX::RxGetPackChannel(2),
            0,
            RCX_CHANNEL_DATA_MAX,
            0,
            255
        );

        /*small*/
        int16_t motorRight = map(
            RCX::RxGetPackChannel(3),
            0,
            RCX_CHANNEL_DATA_MAX,
            0,
            255
        );

        int16_t motorPWM = motorLeft + motorRight * 0.7f;

        static uint16_t motorPWM_Max = 100;
        __IntervalExecute(motorPWM_Max = 100, 20 * 1000);

        if(motorPWM > motorPWM_Max)
        {
            motorPWM_Max = motorPWM;
        }

        MotorLRA_Vibrate(
            fmap(
                motorPWM,
                0, motorPWM_Max,
                motorPWM == 0 ? 0 : 0.25f, 1.0f
            ),
            1000
        );

        static int16_t MotorERMVal = 0;
        int16_t MotorERMVal_Target = 0;
        if(motorPWM > BigVibrateThreshold)
        {
            MotorERMVal_Target = map(motorPWM, BigVibrateThreshold, motorPWM_Max, 0, 100);
            if(MotorERMVal < MotorERMVal_Target)
            {
                MotorERMVal++;
            }
        }
        else
        {
            if(MotorERMVal > MotorERMVal_Target)
            {
                MotorERMVal--;
            }
        }
        MotorERM_SetValue(MotorERMVal);
    }
    else if(state == PBS_Error)
    {
        MotorERM_SetValue(0);
    }
    else if(state == PBS_Exit)
    {
        MotorERM_SetEnable(false);
        MotorLRA_Tone(0);
        MotorLRA_ToneSetVolume(motorToneVolume);
    }
}
