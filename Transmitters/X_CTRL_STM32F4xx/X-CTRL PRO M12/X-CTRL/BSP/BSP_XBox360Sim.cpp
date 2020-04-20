#include "BSP.h"
#include "Communication/ComPrivate.h"

void Passback_XBox360Sim(uint8_t state)
{
    if(state == PBS_Setup)
    {
        BigMotor_SetEnable(true);
    }
    else if(state == PBS_Loop)
    {
        /*BIG*/
        int16_t motorLeft = RCX::GetRxPackChannel(2);
        
        BigMotor_SetValue(
            fmap(
                motorLeft, 
                0, 255, 
                0, 40
             )
        );
        
        /*small*/
        int16_t motorRight = RCX::GetRxPackChannel(3);
        
        static uint16_t motorRight_Max = 100;
        __IntervalExecute(motorRight_Max = 100, 20000);
        
        if(motorRight > motorRight_Max)
        {
            motorRight_Max = motorRight;
        }
        
        Motor_Vibrate(
            fmap(
                motorRight, 
                0, motorRight_Max, 
                motorRight == 0 ? 0 : 0.25f, 1.0f
             ),
             1000
        );
    }
    else if(state == PBS_Error)
    {
        BigMotor_SetValue(0);
    }
    else if(state == PBS_Exit)
    {
        BigMotor_SetEnable(false);
    }
}
