#include "FileGroup.h"
#include "ComPrivate.h"

void Init_Pin()
{
    pinMode(Buzz_Pin, OUTPUT);
    digitalWrite(Buzz_Pin, LOW);
    
//    pinMode(Joystick_X_Pin, INPUT_ANALOG);
//    pinMode(Joystick_Y_Pin, INPUT_ANALOG);
//    pinMode(BattSensor_Pin, INPUT_ANALOG);
}

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))

bool Init_Value()
{
    CTRL.KnobLimit.L = CtrlOutput_MaxValue;
    CTRL.KnobLimit.R = CtrlOutput_MaxValue;
    
    StorageDataReg(JS_L);
    StorageDataReg(JS_R);
    StorageDataReg(CTRL.Info);
    StorageDataReg(State_BuzzSound);
    StorageDataReg(State_PassBack);
    StorageDataReg(State_Handshake);

    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)
    {
        CTRL.Info.CtrlObject = X_COMMON;
        ConnectState.Pattern = Pattern_NRF;

        JS_L.Xmin = 0;
        JS_L.Xmid = ADC_MaxValue / 2;
        JS_L.Xmax = ADC_MaxValue;
        JS_L.Ymin = 0;
        JS_L.Ymid = ADC_MaxValue / 2;
        JS_L.Ymax = ADC_MaxValue;

        JS_R.Xmin = 0;
        JS_R.Xmid = ADC_MaxValue / 2;
        JS_R.Xmax = ADC_MaxValue;
        JS_R.Ymin = 0;
        JS_R.Ymid = ADC_MaxValue / 2;
        JS_R.Ymax = ADC_MaxValue;
        return false;
    }
    return true;
}

void Init_JW_CTRL()
{
    Init_Screen();
    Init_Pin();
    Init_Value();
    Init_BottonEvent();

    bool NRF_IsDetect = false;
    __LoopExecute(NRF_IsDetect = Init_NRF(), 100);
    NRF_IsDetect ? BuzzMusic(4) : BuzzMusic(1);
}
