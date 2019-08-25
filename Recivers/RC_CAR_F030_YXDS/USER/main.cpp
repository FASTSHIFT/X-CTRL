#include "FileGroup.h"

//#define USE_DEBUG

MillisTaskManager mtm(8);

#ifdef USE_DEBUG
void Task_Test()
{
	for(int i=0;i<sizeof(NRF_RxBuff);i++)
	{
		Serial.print(NRF_RxBuff[1],HEX);
	}
	Serial.println();
}
#endif

void setup()
{	
	PWM_Init(LED_Pin, 100, 1000);
	PWM_Init(MotorL_Pin,1000,8000);
	PWM_Init(MotorR_Pin,1000,8000);
	analogWrite(MotorL_Pin, MotorSpeed_MID);
    analogWrite(MotorR_Pin, MotorSpeed_MID);
	
	toneSetTimer(TIM14);
    pinMode(Buzz_Pin, OUTPUT); 
    toneBlock(Buzz_Pin, 523, 80);
    toneBlock(Buzz_Pin, 880, 80);
    toneBlock(Buzz_Pin, 659, 80);
	
    nrf.SetAddress(NRF_AddressConfig);
    nrf.init(RX_MODE, sizeof(NRF_RxBuff));
    while(!nrf.IsDetect())
    {
        toneBlock(Buzz_Pin, 100, 500);
        delay(500);
    }
	
    mtm.TaskRegister(0, Task_NRF_Receive, 10);
    mtm.TaskRegister(1, Task_CarCtrl, 5);
	mtm.TaskRegister(2, Task_BuzzRing_Block, 100);
	mtm.TaskRegister(3, Task_MID_Setting, 100);

#ifdef USE_DEBUG
    Serial.begin(115200);
	mtm.TaskRegister(4, Task_Test, 30);
#endif
}

void loop()
{
    mtm.Running(millis());
}

//*****************MAIN******************//

void BaseClocks_Init()
{
	RCC_HSICmd(ENABLE);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	while(RCC_GetSYSCLKSource());
	RCC_PLLCmd(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_12);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLKConfig(RCC_SYSCLK_Div1);
}

int main(void)
{
	BaseClocks_Init();
    Delay_Init();
//  ADCx_Init(ADC1);
//	ADCx_DMA_Init();
    setup();
    for(;;)loop();
}
