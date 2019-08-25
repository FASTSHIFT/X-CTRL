#include "FileGroup.h"

void Init_X_CTRL()
{
	BlueTooth.begin(115200);
	Init_Pin();
	Init_TFT();
	Init_Value();
	if(ConnectState.Pattern == Pattern_NRF) Init_NRF(CTRL.Info.CtrlObject);
}

void Init_Pin()
{
//	pinMode(JSL_X_Pin, INPUT_ANALOG);
//	pinMode(JSL_Y_Pin, INPUT_ANALOG);
//	pinMode(JSR_X_Pin, INPUT_ANALOG);
//	pinMode(JSR_Y_Pin, INPUT_ANALOG);
//	pinMode(ADL_Pin, INPUT_ANALOG);
//	pinMode(ADR_Pin, INPUT_ANALOG);
//	pinMode(BattSensor_Pin, INPUT_ANALOG);
//	Use DMA

    pinMode(EncoderLED_Pin, PWM);
    pinMode(EncoderKey_Pin, INPUT_PULLUP);
    pinMode(EncoderA_Pin, INPUT);
    pinMode(EncoderB_Pin, INPUT);

    pinMode(Buzz_Pin, OUTPUT);
	digitalWrite(Buzz_Pin, LOW);
    pinMode(SPDT_Switch_Pin, INPUT_PULLDOWN);

    pinMode(HC165_OUT_Pin, INPUT);
    pinMode(HC165_CP_Pin, OUTPUT);
    pinMode(HC165_PL_Pin, OUTPUT);

    pinMode(BT_PowerCtrl_Pin, OUTPUT);
    pinMode(BT_State_Pin, INPUT);
    pinMode(BT_EN_Pin, OUTPUT);
}

void Init_TFT()
{
	tft.initR(INITR_MINI128x64);
    tft.fillScreen(ST7735_BLACK);
	tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
    tft.setTextWrap(false);
    tft.setTextSize(1);
    tft.drawRect(0, 8, 50, 50, ST7735_WHITE);
    tft.drawRect(127 - 50, 8, 50, 50, ST7735_WHITE);
}

void Init_NRF(uint8_t CtrlObject)
{
	switch(CtrlObject)
	{
		case X_COMMON:
			nrf.SetAddress(NRF_Address_Common);
			nrf.init(TX_MODE, (uint8_t*)&CTRL,sizeof(CTRL));
		break;
		
		case CAR_DifferentalSteering:
			nrf.SetAddress(NRF_Address_CarDS);
			nrf.init(TX_MODE, (uint8_t*)&CarDS,sizeof(CarDS));
		break;
		
		case CAR_ServoSteering:
			nrf.SetAddress(NRF_Address_CarSS);
			nrf.init(TX_MODE, (uint8_t*)&CarSS,sizeof(CarSS));
		break;
	}
}

void Init_Value()
{
	CTRL.Info.CtrlObject = CAR_ServoSteering;//X_COMMON;
	ConnectState.Pattern = Pattern_NRF;

	
	CTRL.KnobLimit.L = map(analogRead_DMA(ADL_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
	CTRL.KnobLimit.R = map(analogRead_DMA(ADR_Pin), 0, ADC_MaxValue, 0, CtrlOutput_MaxValue);
	
	JS_L.Xmin=0;
	JS_L.Xmid=4095/2;
	JS_L.Xmax=4095;
	JS_L.Ymin=0;
	JS_L.Ymid=4095/2;
	JS_L.Ymax=4095;
	
	JS_R.Xmin=0;
	JS_R.Xmid=4095/2;
	JS_R.Xmax=4095;
	JS_R.Ymin=0;
	JS_R.Ymid=4095/2;
	JS_R.Ymax=4095;
}
