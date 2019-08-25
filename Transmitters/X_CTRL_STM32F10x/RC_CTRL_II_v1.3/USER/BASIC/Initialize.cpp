#include "FileGroup.h"

void Init_Module()
{
    Wire.begin();
    hc05.Init(115200);
    for(uint8_t i = 0; i < 100; i++)mpu.initialize();
    oled.begin(SSD1306_SWITCHCAPVCC);
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.display();
    hc05.Init(115200);
}

void Init_GPIO()
{
    pinMode(JSL_X_Pin, INPUT_ANALOG);
    pinMode(JSL_Y_Pin, INPUT_ANALOG);
    pinMode(JSR_X_Pin, INPUT_ANALOG);
    pinMode(JSR_Y_Pin, INPUT_ANALOG);
    pinMode(ADL_Pin, INPUT_ANALOG);
    pinMode(ADR_Pin, INPUT_ANALOG);
}

void Init_Value()
{
    CTRL.KnobLimit.L = 0;
    CTRL.KnobLimit.R = 0;

    if(EEPROM_Handle(ReadData) == false)
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
    }
    CTRL.Info.CtrlMode = CtrlMode_Default;
}


void Init_RC_Ctrl()
{
    Init_Module();
    Init_GPIO();
    Init_Value();
    Init_Button();
	Init_Buzz();

    if(!digitalRead(Button_OK_Pin))
    {
        delay(20);
        if(!digitalRead(Button_OK_Pin))
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
