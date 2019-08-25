#include "FileGroup.h"

typedef enum {
    None,
    Fan,
    LED,
    Car
} CtrlObject_Type;

CtrlObject_Type NowCtrlObj;

const uint8_t InstructSet[2][8] = {
    {WAND_DOWN, WAND_RIGHT, 0},
    {WAND_RIGHT, WAND_DOWN, WAND_LEFT, WAND_UP, 0}
};

void Sendinstructions_Handler()
{

}

void Modify_Handler()
{
    if(RC_X.dataMark == UD)
    {
        if(NowCtrlObj == LED)
        {
            static int LED_PWM = 0;
            analogWrite(LED_Pin, LED_PWM);
            if(RC_X.dataPack.udMotion > 0)LED_PWM++;
            else if(RC_X.dataPack.udMotion < 0)LED_PWM--;
            LimitValue(LED_PWM, 0, CtrlOutput_MaxValue);
        }
        else if(NowCtrlObj == Fan)
        {
            static int Fan_PWM = 0;
            analogWrite(Fan_Pin, Fan_PWM);
            if(RC_X.dataPack.udMotion > 0)Fan_PWM++;
            else if(RC_X.dataPack.udMotion < 0)Fan_PWM--;
            LimitValue(Fan_PWM, 0, CtrlOutput_MaxValue);
        }
    }
    else if(RC_X.dataMark == JOYSTICK)
    {
        
    }
}

void Task_Ctrl()
{
    if(ConnectState.IsConnect && ConnectState.IsCorrect)
    {
        switch(RC_X.mode)
        {
        case FREEDOM:
            break;
        case SENDINSTRUCTIONS:
            Sendinstructions_Handler();
            break;
        case MODIFY:
            Modify_Handler();
            break;
        }
    }
    else
    {
        static uint16_t LED_PWM = 0;
        analogWrite(LED_Pin, LED_PWM);
        LED_PWM+=50;
        LED_PWM %= 1000;
    }
}
