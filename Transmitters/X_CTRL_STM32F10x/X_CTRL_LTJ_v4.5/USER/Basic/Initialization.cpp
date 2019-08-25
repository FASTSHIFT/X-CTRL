#include "FileGroup.h"

void Init_Pin()
{
    pinMode(EncoderLED_Pin, PWM);
    analogWrite(EncoderLED_Pin, 0);
    pinMode(EncoderA_Pin, INPUT);
    pinMode(EncoderB_Pin, INPUT);

    pinMode(Buzz_Pin, OUTPUT);
    digitalWrite(Buzz_Pin, LOW);
    pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);

    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);
}

void Init_HC05()
{
    hc05.Init(115200);
    hc05.Power(State_Bluetooth);
}

#define StorageDataReg(data) EEPROM_Register(&(data), sizeof(data))

bool Init_Value()
{
    CTRL.KnobLimit.L = 0;
    CTRL.KnobLimit.R = 0;

    StorageDataReg(JS_L);
    StorageDataReg(JS_R);
    StorageDataReg(CTRL.Info);
    StorageDataReg(NRF_Cfg);
    StorageDataReg(State_BuzzSound);
    StorageDataReg(State_PassBack);
    StorageDataReg(State_Handshake);
    StorageDataReg(State_Bluetooth);
    StorageDataReg(State_PowerSaving);
    StorageDataReg(Bluetooth_ConnectObject);

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

void Init_X_CTRL()
{
    Init_Display();
    Init_Pin();
    //Init_MPU6050();
    Init_Value();
    Init_BottonEvent();
    Init_EncoderEvent();
    Init_HC05();

//    if(!digitalRead(EncoderKey_Pin))
//    {
//        delay(20);
//        if(!digitalRead(EncoderKey_Pin))
//            State_BuzzSound = OFF;
//    }

    if(ConnectState.Pattern == Pattern_NRF)
    {
        bool NRF_IsDetect = false;
        __LoopExecute(NRF_IsDetect = Init_NRF(), 100);
        NRF_IsDetect ? BuzzMusic(0) : BuzzMusic(1);
    }
}
