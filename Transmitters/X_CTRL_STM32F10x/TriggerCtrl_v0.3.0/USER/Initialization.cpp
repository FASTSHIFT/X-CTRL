#include "FileGroup.h"

void Init_Pin()
{
    pinMode(Buzz_Pin, OUTPUT);
    digitalWrite(Buzz_Pin, LOW);

    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);
    pinMode(SW_L_Pin, INPUT);
    pinMode(SW_R_Pin, INPUT);

    pinMode(JSL_X_Pin, INPUT_ANALOG);
    pinMode(JSL_Y_Pin, INPUT_ANALOG);
    pinMode(JSR_X_Pin, INPUT_ANALOG);
    pinMode(JSR_Y_Pin, INPUT_ANALOG);
    pinMode(ADR_Pin, INPUT_ANALOG);
    pinMode(ADL_Pin, INPUT_ANALOG);
    pinMode(BattSensor_Pin, INPUT_ANALOG);
}

//void Init_HC05()
//{
//    hc05.Init(115200);
//    hc05.Power(ON);
//}

void Init_Value()
{
    CTRL.Info.CtrlObject = X_COMMON;//CAR_ServoSteering;
    ConnectState.Pattern = Pattern_NRF;

    CTRL.KnobLimit.L = map(analogRead(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
    CTRL.KnobLimit.R = map(analogRead(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);

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

void Init_TriggerCtrl()
{
    Init_Pin();
    Init_Value();
	Init_ButtonEvent();
	PageRegister_MainMenu();
	PageRegister_CtrlInfo();

    if(ConnectState.Pattern == Pattern_NRF)
    {
        for(uint8_t i = 0; i < 100; i++)
        {
            Init_NRF();
        }
    }
    else if(ConnectState.Pattern == Pattern_USART)
    {
//        Init_HC05();
    }
	
	Serial.begin(115200);
    Wire.begin();
    mpu.initialize();
}
