#include "FileGroup.h"
#include "ComPrivate.h"



void Passback_XBox360Sim_Handler()
{
    static int16_t motorMaxVal = 100;
    
    __IntervalExecute(motorMaxVal = 100, 20 * 1000);
    
    /*BIG*/
    int16_t motorLeft = RCX::GetRxPackChannel(2);
    
    /*small*/
    int16_t motorRight = RCX::GetRxPackChannel(3);
    
    int16_t motorPwm = motorLeft + motorRight * 0.7f;
    
    if(motorPwm > motorMaxVal)
    {
        motorMaxVal = motorPwm;
    }
    
    MotorVibrate(
        fmap(
            motorPwm, 
            0, motorMaxVal, 
            motorPwm == 0 ? 0 : 0.15f, 1.0f),
        1000
    );
}
