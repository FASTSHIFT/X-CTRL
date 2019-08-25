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

void Init_HC05()
{
    hc05.Init(115200);
    hc05.Power(ON);
}

void Init_Value()
{
    CTRL.KnobLimit.L = 0;
    CTRL.KnobLimit.R = 0;

    if(EEPROM_Handle(ReadData) == false)
    {
        if(digitalRead(SPDT_Switch_Pin))
        {
            CTRL.Info.CtrlObject = CAR_ServoSteering;
        }
        else
        {
            CTRL.Info.CtrlObject = CAR_DifferentalSteering;
            CTRL.Info.CtrlMode = CtrlMode_Race;
        }

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
    }

    CTRL.Info.CtrlMode = CtrlMode_Default;
}

void Init_X_CTRL()
{
    Init_Display();
    Init_Pin();

    Wire.begin();
    for(uint8_t i = 0; i < 100; i++)
    {
        mpu.initialize();
    }

    Init_Value();
    Init_BottonEvent();
    Init_EncoderEvent();
    Init_HC05();

    if(!digitalRead(EncoderKey_Pin))
    {
        delay(20);
        if(!digitalRead(EncoderKey_Pin))
            BuzzSound_State = OFF;
    }

    if(ConnectState.Pattern == Pattern_NRF)
    {
        for(uint8_t i = 0; i < 100; i++)
        {
            Init_NRF();
        }

        if(nrf.IsDetect())
        {
            BuzzMusic(4);
        }
        else
        {
            BuzzMusic(1);
        }
    }
}
