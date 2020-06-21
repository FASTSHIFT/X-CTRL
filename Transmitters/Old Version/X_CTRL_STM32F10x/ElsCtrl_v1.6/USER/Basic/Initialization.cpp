#include "FileGroup.h"
#include "ComPrivate.h"

void Init_Pin()
{
    pinMode(Buzz_Pin, OUTPUT);
    pinMode(InfoLED_Pin, OUTPUT);
    pinMode(ChargeDetect_Pin, INPUT_PULLUP);
    pinMode(BattSensor_Pin, INPUT_ANALOG);
    pinMode(JSL_X_Pin, INPUT_ANALOG);
    pinMode(JSL_Y_Pin, INPUT_ANALOG);
    
    digitalWrite(Buzz_Pin, LOW);
}

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))
    
bool Init_Value()
{
    StorageDataReg(CTRL.KnobLimit);
    StorageDataReg(JS_L);
    StorageDataReg(CTRL.Info);
    StorageDataReg(NRF_Cfg);
    StorageDataReg(State_BuzzSound);
    //StorageDataReg(State_PassBack);
    StorageDataReg(State_Handshake);
    StorageDataReg(NRF_AddressConfig);

    if(EEPROM_Handle(EEPROM_Chs::ReadData) == false)
    {
        CTRL.KnobLimit.L = CtrlOutput_MaxValue;
        CTRL.KnobLimit.R = CtrlOutput_MaxValue;
        CTRL.Info.CtrlObject = X_COMMON;
        ConnectState.Pattern = Pattern_NRF;

        JS_L.Xmin = 0;
        JS_L.Xmid = ADC_MaxValue / 2;
        JS_L.Xmax = ADC_MaxValue;
        JS_L.Ymin = 0;
        JS_L.Ymid = ADC_MaxValue / 2;
        JS_L.Ymax = ADC_MaxValue;
        
        return false;
    }
    return true;
}

void Init_ElsCtrl()
{
    Init_Pin();
    Init_Value();
    Init_BottonEvent();

    if(!digitalRead(Button_OK_Pin))
    {
        delay(20);
        if(!digitalRead(Button_OK_Pin))
            State_BuzzSound = OFF;
    }

    bool NRF_IsDetect = false;
    __LoopExecute(NRF_IsDetect = Init_NRF(), 100);
    NRF_IsDetect ? BuzzMusic(0) : BuzzMusic(1);
    
    pinMode(NRF_IRQ_Pin, INPUT_PULLUP);
    EXTIx_Init(NRF_IRQ_Pin, NRF_IRQHandler, FALLING, 1, 1);
    
    Init_Display();
}
