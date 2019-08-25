#include "FileGroup.h"

void Init_Pin()
{
    pinMode(EncoderLED_Pin, PWM);
    analogWrite(EncoderLED_Pin, 0);

    pinMode(Buzz_Pin, OUTPUT);
    digitalWrite(Buzz_Pin, LOW);
    pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);

    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);
}

void Init_NRF(uint8_t CtrlObject)
{
    switch(CtrlObject)
    {
    case X_COMMON:
        nrf.SetAddress(NRF_Address_Common);
        nrf.init(TX_MODE, (uint8_t*)&CTRL, sizeof(CTRL));
        break;

    case CAR_DifferentalSteering:
        nrf.SetAddress(NRF_Address_CarDS);
        nrf.init(TX_MODE, (uint8_t*)&CarDS, sizeof(CarDS));
        break;

    case CAR_ServoSteering:
        nrf.SetAddress(NRF_Address_CarSS);
        nrf.init(TX_MODE, (uint8_t*)&CarSS, sizeof(CarSS));
        break;
    }
}

void Init_HC05()
{
    hc05.Init(115200);
    hc05.Power(ON);
}

void Init_Value()
{
    CTRL.Info.CtrlObject = X_COMMON;//CAR_ServoSteering;
    ConnectState.Pattern = Pattern_NRF;

    CTRL.KnobLimit.L = map(analogRead_DMA(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
    CTRL.KnobLimit.R = map(analogRead_DMA(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);

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
}

void Init_X_CTRL()
{
    Init_Pin();
    Init_Value();
    Init_Display();
    Init_BottonEvent();
    Init_EncoderEvent();

    if(!digitalRead(EncoderKey_Pin))SilentMode = ON;

    if(ConnectState.Pattern == Pattern_NRF)
    {
        for(uint8_t i = 0; i < 100; i++)
        {
            Init_NRF(CTRL.Info.CtrlObject);
        }

        if(nrf.IsDetect())
        {
            BuzzRing(4);
        }
        else
        {
            BuzzRing(1);
        }
    }
    else if(ConnectState.Pattern == Pattern_USART)
    {
        Init_HC05();
    }

    Wire.begin();
    mpu.initialize();
}
